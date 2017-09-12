#include "Server.h"
#define msg_sz sizeof(Message)

// Auxiliary Classes
Game game;
SharedMemoryHelper smHelper;

// Shared Memory Handles
HANDLE hThreadSharedMemory;
HANDLE hThreadSharedMemoryReader;
DWORD dwThreadSMReader = 0;
HANDLE hThreadSharedMemoryWriter;
DWORD dwThreadSMWriter = 0;

// Shared Memory Threads
bool threadSharedMemFlag;
bool threadReadFromSMFlag = false;
bool threadWriteFromSMFlag = false;
unsigned int smThreadID;

// Server Pipe
HANDLE serverPipe;
HANDLE serverPipeAdmin;

LPCTSTR lpName;
DWORD dwOpenMode;
DWORD dwPipeMode;
DWORD nMaxInstances;
DWORD nOutBufferSize;
DWORD nInBufferSize;
DWORD nDefaultTimeOut;
LPSECURITY_ATTRIBUTES lpSecurityAttributes;

LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");
LPTSTR lpszPipeNameAdmin = TEXT("\\\\.\\pipe\\pipeAdmin");
HANDLE WriteReady;

// Client's Array HANDLES
HANDLE clients[MAX_PLAYERS];
int numPlayersConnected = 0;

HANDLE hThread;
HANDLE hThreadReceiveClients;

BOOL fConnected = FALSE;
DWORD dwThreadId = 0;

HANDLE hGamePhaseEvent;



// Constructor
Server::Server()
{
	game = Game();

	// Create Pipe

	lpName = TEXT("\\\\.\\pipe\\exemplo");
	dwOpenMode = PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED;
	dwPipeMode = PIPE_TYPE_MESSAGE;
	nMaxInstances = PIPE_UNLIMITED_INSTANCES;
	nOutBufferSize = BUFSIZE;
	nDefaultTimeOut = 5000;
	lpSecurityAttributes = NULL;

	// Connect Pipe

	serverPipe = Create();

	WriteReady = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (WriteReady == NULL) {
		_tprintf(TEXT("\nServidor: nao foi possivel criar o write"));
		exit(1);
	}

	initializeClients();

}

Server::~Server()
{
}

// Shared Memory Functions
DWORD WINAPI ReadFromSharedMemory(LPVOID lParam) {

	HMODULE hDLL = LoadLibrary(TEXT("SM_DLL.dll"));
	Message * (*ptr)(void);

	if (hDLL == NULL) {
		cout << "não Há dll" << endl;
	}
	ptr = (Message * (*)(void)) GetProcAddress(hDLL, "ReadFromSharedMemoryBuffer");
	if (ptr == NULL) {
		//tcout << TEXT("ptr não tem o metodo ReadFromSharedMemoryBuffer") << endl;
		return -1;
	}

	while (1) {
		Message * msg = ptr();
		if (msg != nullptr)
			//tcout << TEXT(msg->msg) << TEXT(msg->pid) << endl;

			if (threadReadFromSMFlag) {
				return 1;
			}

	}

	return 1;
}

DWORD WINAPI WriteForSharedMemory(LPVOID lParam) {


	HMODULE hDLL = LoadLibrary(TEXT("SM_DLL.dll"));
	bool * (*ptr)(Message);

	if (hDLL == NULL) {
		cout << "não Há dll" << endl;
	}
	ptr = (bool * (*)(Message)) GetProcAddress(hDLL, "WriteToSharedMemoryBuffer");
	if (ptr == NULL) {
		tcout << TEXT("ptr não tem o metodo WriteToSharedMemoryBuffer") << endl;
		return -1;
	}

	//while (1) //{

		//if (ptr(game.exportInfoToMessage())) {
			//tcout << TEXT("ENviado com sucesso") << endl;
		//}
		//else {
			//tcout << TEXT("Erro ao enviar") << endl;
		//}

		//if (threadWriteFromSMFlag) {
	return 1;
	//}

//}

	return 1;
}

unsigned int __stdcall ThreadSharedMemoryReader(void * p)
{
	Server * server = (Server *)p;

	CreateThread(
		NULL,
		0,
		ReadFromSharedMemory,
		(LPVOID)hThreadSharedMemoryReader,
		0,
		&dwThreadSMReader);

	CreateThread(
		NULL,
		0,
		WriteForSharedMemory,
		(LPVOID)hThreadSharedMemoryWriter,
		0,
		&dwThreadSMWriter);

	while (server->getSharedMemFlag()) {
		//Ler da memoria partilhada e imprimir no ecra

	}

	threadReadFromSMFlag = true;
	threadSharedMemFlag = true;

	tcout << "ThreadSharedMemoryReader Closed" << endl;
	return 0;
}

bool Server::getSharedMemFlag() const {
	return threadSharedMemFlag;
}

// Server Phases Function
void Server::startServer()
{

	if (!smHelper.initSharedMemory()) {
		finishServer();
	}

	//lançar a thread the leitura
	threadSharedMemFlag = true;
	//_beginthreadex(0, 0, ThreadSharedMemoryReader, this, 0, &smThreadID);
	//hThreadSharedMemory = OpenThread(THREAD_ALL_ACCESS, FALSE, smThreadID);

	serverMainLoop();
}

void Server::serverMainLoop()
{

	hGamePhaseEvent = CreateEvent(
		NULL,              
		TRUE,               
		FALSE,             
		TEXT("GamePhaseEvent"));

	tcout << "Server Online." << endl;
	game.setInitalPhase();
	//iniciar thread de aceitar clientes

		//Initial Phase		
	_tprintf(TEXT("\nFase inicial iniciada\n"));
	initialPhaseLoop();

	WaitForSingleObject(hGamePhaseEvent, INFINITE);
	//Game Phase
	_tprintf(TEXT("\nFase de jogo iniciada\n"));
	GamePhaseLoop();


	Message msg;
	msg.code = END;
	sprintf(msg.msg, "Game finished");
	Broadcast(msg);
	//Finish Phase

	CloseHandle(hGamePhaseEvent);
	finishServer();
}

void Server::initialPhaseLoop()
{
	waitConnection();
}

void Server::GamePhaseLoop()
{
	startAdminPipe();
	tcout << "Game Phase Loop started" << endl;
	do {
		game.updateMap();
		game.exportInfoToMessage();
		Broadcast(game.exportInfoToMessage());
		Sleep(33); //Fazer 30 atualizações por segundo (30 FPS)
	} while (game.getGamePhase() == IN_PROGRESS_PHASE);
}

void Server::finishServer()
{
	_tprintf(TEXT("Server Exit, see ya!\n"));
	threadSharedMemFlag = false;
	WaitForSingleObject(hThreadSharedMemory, INFINITE);
	smHelper.finishSharedMemory();
	CloseHandle(hThreadSharedMemory);
}

void Server::startGame()
{
	game.setInProgressPhase();
	game.initMap();
	SetEvent(hGamePhaseEvent);
}


// Support Command Functions
int Server::commandParser(vector<string> command, Message msg)
{

	if (command.size() <= 0)
		return FAIL;
	switch (msg.code) {
	case START:
		if (game.getGamePhase() == INITIAL_PHASE) {
			return START;
		}
		return FAIL;
		break;

		//CREATEGAME <Map Width [1,80]> <Map Height [1,80]> <Nr Players [1,10]> <Snake Size [1,10]> <Nr Objects [1,10]> <Nr Snakes AI [1,10]> <Player Username>
	case CREATEGAME:
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 7) {
				if (stoi(command[0]) >= 1 && stoi(command[0]) <= MAX_TAM_MAP) {
					if (stoi(command[1]) >= 1 && stoi(command[1]) <= MAX_TAM_MAP) {
						if (stoi(command[2]) >= 1 && stoi(command[2]) <= MAX_PLAYERS) {
							if (stoi(command[3]) >= 1 && stoi(command[3]) <= MAX_PLAYERS) {
								if (stoi(command[4]) >= 1 && stoi(command[4]) <= MAX_PLAYERS) {
									if (stoi(command[5]) >= 1 && stoi(command[5]) <= MAX_PLAYERS) {
										if (command[6] != "") {
											return CREATEGAME;
										}
										else {
											cout << "COMMAND - Error in Username" << endl;

										}
									}
									else {
										cout << "COMMAND - Error in Nr Snakes AI" << endl;

									}
								}
								else {
									cout << "COMMAND - Error in Nr Objects" << endl;
								}
							}
							else {
								cout << "COMMAND - Error in Snake Size" << endl;
							}
						}
						else {
							cout << "COMMAND - Error in Nr Players" << endl;
						}
					}
					else {
						cout << "COMMAND - Error in Map Height" << endl;
					}
				}
				else {
					cout << "COMMAND - Error in Map Width" << endl;
				}
				return FAIL;
			}
			return FAIL;
		}
		return FAIL;
		break;

		//JOIN <playerName>
	case JOIN:
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 1)
				if (command[0] != "")
					return JOIN;
				else
					cout << "COMMAND - Error in Username" << endl;
			return FAIL;
		}
		return FAIL;
		break;
		//SETDIRECTION <Direction(1,2,3,4)>
	case SETDIRECTION:
		if (command.size() == 1) {
			if (game.getGamePhase() == IN_PROGRESS_PHASE)
				if (stoi(command[0]) > 0 && stoi(command[0]) < GOING_RIGHT)
					return SETDIRECTION;
				else
					cout << "COMMAND - Error in Direction" << endl;
			return FAIL;
		}
		return FAIL;
		break;//DISCONNECT <PID>
	case DISCONNECT:
		if (command.size() == 1) {
			char* p;
			strtol(command[0].c_str(), &p, 10);
			if (*p == 0)
				return DISCONNECT;
			else
				cout << "COMMAND - Error in PID" << endl;
			return FAIL;
		}
		return FAIL;
		break;
	case SEED_OBJECT:
		if (command.size() == 2) {
			if (stoi(command[0]) >= 2 && stoi(command[0]) <= COFFEE_BLOCK)
				if (stoi(command[0]) >= 1 && stoi(command[0]) <= 10)//maximo de 10 objectos postos
					return SEED_OBJECT;
				else
					return FAIL;
			else
				return FAIL;
		}
		return FAIL;
		break;
	default:
		//WRONG COMMAND
		return FAIL;
		break;
	}
	return FAIL;
}

void Server::treatCommand(vector<string> command, Message msg)
{
	switch (msg.code) {

	case START:
		game.setInProgressPhase();
		Message response;
		response.code = START;
		sprintf(response.msg, "Game Started");
		Broadcast(response);
		break;

	case CREATEGAME:
		game.setMapHeight(10);
		game.setMapWidth(10);
		game.setNumSnakesAI(3);
		game.setNumberOfObjects(3);
		game.setSnakeSize(3);
		game.addPlayer(1, "jorge");
		startGame();


		/*game.setMapWidth(stoi(command[0]));
		game.setMapHeight(stoi(command[1]));
		game.setNumPlayers(stoi(command[2]));
		game.setSnakeSize(stoi(command[3]));
		game.setNumberOfObjects(stoi(command[4]));
		game.setNumSnakesAI(stoi(command[5]));
		game.addPlayer(new Player(msg.pid, command[6], &game));*/
		break;

	case JOIN:
		game.addPlayer(new Player(msg.pid, command[0], &game));
		break;

	case SETDIRECTION:
		game.setDirectionToPlayer(msg.pid, stoi(command[0]));
		break;

	case DISCONNECT:
		game.removePlayer(msg.pid);
		break;

	case SEED_OBJECT:
		game.addSpecialBlock(stoi(command[0]), stoi(command[1]));
		break;

	}
}

vector<string> Server::getCommand(char* buffer)
{

	string commandString = buffer;
	commandString = commandToUpperCase(commandString);
	//Vectorize command
	vector <string> comand;
	string temp;
	stringstream ss(commandString);
	while (ss >> temp)
		comand.push_back(temp);

	return comand;
}

string Server::commandToUpperCase(string command)
{
	for (unsigned int i = 0; i < command.size(); i++) {
		if (command[i] >= 'a' && command[i] <= 'z') {
			command[i] -= 32;
		}
	}
	return command;
}


int Server::waitConnection()
{
	hThreadReceiveClients = CreateThread(
		NULL,
		0,
		ThreadReceiveClients,
		NULL,
		0,
		NULL);

	if (hThreadReceiveClients == NULL) {
		_tprintf(TEXT("\nErro na criacao da thread. Erro = %d"), GetLastError());
	}
	else
		CloseHandle(hThreadReceiveClients);
	return 1;
}

HANDLE Server::Create() {
	return CreateNamedPipe(lpName,
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);
}

BOOL Server::Connect() {
	return ConnectNamedPipe(serverPipe, NULL);
}

BOOL Server::Disconnect() {
	return DisconnectNamedPipe(serverPipe);
}

int Server::Write(HANDLE hPipe, Message msg) {

	DWORD cbWritten = 0;
	BOOL fSuccess = 0;

	OVERLAPPED OverlWr = { 0 };

	ZeroMemory(&OverlWr, sizeof(OverlWr));  // não é necessário pq se inicializa com {0} mas mete-se na mesma
	ResetEvent(WriteReady); // não assinalado
	OverlWr.hEvent = WriteReady; // TENHO DE MUDAR ISTO!!!

	fSuccess = WriteFile(
		hPipe,	// handle para o pipe
		&msg,	// message(ponteiro)
		msg_sz,	// tamanho da mensagem
		&cbWritten,	// ptr p/ guardar num bytes escritos
		&OverlWr);	// != NULL -> É mesmo overlapped I/O

	WaitForSingleObject(WriteReady, INFINITE);

	GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);
	return 1;

}

// Recebe o HANDLE e adiciona o cliente à lista de clientes
void Server::addClient(HANDLE cli) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == INVALID_HANDLE_VALUE) {
			clients[i] = cli;
			numPlayersConnected++;
			return;
		}
	}

}

void Server::rmClient(HANDLE cli) {

	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == cli) {
			clients[i] = INVALID_HANDLE_VALUE;
			numPlayersConnected--;
			return;
			;
		}
	}
}

int Server::Broadcast(Message msg) {

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] != 0)
			Write(clients[i], msg);
	}

	return 0;

}

void Server::setfConnected(BOOL flag) {

	fConnected = flag;

}

void Server::initializeClients()
{
	for (int i = 0; i < MAX_PLAYERS; i++)
		clients[i] = INVALID_HANDLE_VALUE;
}

void Server::setHNamedPipe(HANDLE namedPipe) {

	serverPipe = namedPipe;

}

HANDLE Server::getHNamedPipe()
{
	return HANDLE();
}

void Server::startAdminPipe()
{
	HANDLE hThread;

	hThread = CreateThread(
		NULL,
		0,
		ThreadProcAdmin,
		NULL,
		0,
		NULL);

	if (hThread == NULL) {
		_tprintf(TEXT("\nErro na criacao da thread. Erro = %d"), GetLastError());
	}
	else
		CloseHandle(hThread);

}

DWORD WINAPI Server::ThreadReceiveClients(LPVOID lbpParam)
{
	// Remote pipe atributes ->
	TCHAR * szSD = TEXT("D:")	// D -> Discretionary ACL (O,G,D,S)
		TEXT("A;OICI;GA;;;BG")		// A -> Allow Generic Access a built-in guests (D -> Deny)
		TEXT("(A;OICI;GA;;;AN)")	// Allow access a Anonymous logon
		TEXT("(A;OICI;GA;;;AU)")	// Allow Authenticated Users R/W/X (Generic Access)
		TEXT("(A;OICI;GA;;;BA)");	// Allow access a Built-in Administrator

	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	PSID everyone_sid = NULL;
	AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &everyone_sid);

	EXPLICIT_ACCESS ea;
	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance = NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	//ea.Trustee.ptstrName = (LPWSTR)everyone_sid;

	PACL acl = NULL;
	SetEntriesInAcl(1, &ea, NULL, &acl);

	// Security Descriptor
	PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE);

	// Security Attributes
	SECURITY_ATTRIBUTES sa; // atributos para o pipe remoto
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = sd;
	sa.bInheritHandle = FALSE;


	ConvertStringSecurityDescriptorToSecurityDescriptor(
		szSD,
		SDDL_REVISION_1,
		//&(pSA->lpSecurityDescriptor),
		&sa.lpSecurityDescriptor, // isto nao corresponde com o que está nos slides
		NULL);

	// <- Remote pipe atributes

	// Enter the cicle(Main Thread)
	_tprintf(TEXT("\nServidor a aceitar clientes"));


	while (1) {

		if (numPlayersConnected > MAX_PLAYERS) {
			break;
		}

		if (game.getGamePhase() != INITIAL_PHASE) {
			break;
		}

		if (numPlayersConnected == MAX_PLAYERS) {
			// Exit the loby and start the game

			startGame();
		}

		serverPipe = CreateNamedPipe(
			lpszPipename, // nome do pipe
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE,
			BUFSIZE,
			5000,
			&sa); // Use NULL to make the pipe local - Use a reference to make a network pipe(&sa is a pointer to a security attribute)	

		if (serverPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("\nFalhou a criacao do pipe, erro = %d"), GetLastError());
			return -1;
		}


		fConnected = ConnectNamedPipe(serverPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) {

			_tprintf(TEXT("\n%d Jogadores no servidor"), numPlayersConnected);

			hThread = CreateThread(
				NULL,
				0,
				ThreadProcClient,
				(LPVOID)serverPipe,
				0,
				&dwThreadId);

			addClient(hThread);

			if (hThread == NULL) {
				_tprintf(TEXT("\nErro na criacao da thread. Erro = %d"), GetLastError());

				return -1;
			}
			else
				CloseHandle(hThread);

		}
		else
			CloseHandle(serverPipe);

	}
	return 0;
}

DWORD WINAPI Server::ThreadProcClient(LPVOID lpvParam)
{

	Message clientRequest;
	DWORD cbBytesRead = 0, cbReplyBytes = 0;
	int numresp = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a infroamacao enviada à thread é o handle do pipe

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	if (hPipe == NULL) {
		_tprintf(TEXT("\nErro - o handle enviado no param da thread é nulo"));
		return -1;
	}

	ReadReady = CreateEvent(
		NULL,	// default 
		TRUE,
		FALSE,
		NULL);

	if (ReadReady == NULL) {
		_tprintf(TEXT("\nServidor: não foi possível criar o evento Read. Mais vale parar já"));
		return 1;
	}


	while (threadSharedMemFlag) {

		ZeroMemory(&OverlRd, sizeof(OverlRd));
		ResetEvent(ReadReady);
		OverlRd.hEvent = ReadReady;

		fSuccess = ReadFile(
			hPipe,
			&clientRequest,
			msg_sz,
			&cbBytesRead,
			&OverlRd);

		WaitForSingleObject(ReadReady, INFINITE);

		GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);

		if (cbBytesRead < msg_sz) {
			//nao leu tudo do readFile
			_tprintf(TEXT("\nErro na leitura do pipe, erro = %d"), GetLastError());
			break;
		}
		else {
			vector<string> command = getCommand(clientRequest.msg);


			switch (commandParser(command, clientRequest))
			{
			case START:
				treatCommand(command, clientRequest);
				clientRequest.code = START;
				sprintf(clientRequest.msg, "Game started");
				Write(hPipe, clientRequest);
				break;

			case CREATEGAME:
				treatCommand(command, clientRequest);
				clientRequest.code = CREATEGAME;
				sprintf(clientRequest.msg, "Game created with success");
				Write(hPipe, clientRequest);
				break;

			case JOIN:
				treatCommand(command, clientRequest);
				sprintf(clientRequest.msg, "You joined in a created game");
				clientRequest.code = JOIN;
				Write(hPipe, clientRequest);
				break;

			case SETDIRECTION:
				treatCommand(command, clientRequest);
				break;

			case DISCONNECT:
				treatCommand(command, clientRequest);
				sprintf(clientRequest.msg, "Client has to close");
				clientRequest.code = DISCONNECT;
				Write(hPipe, clientRequest);
				break;

			case FAIL:
				break;
			default:
				break;
			}


		}
	}//end while

	rmClient(hPipe);
	FlushFileBuffers(hPipe);
	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	_tprintf(TEXT("\nThread dedicada Cliente a terminar"));
	return 1;

}

DWORD Server::ThreadProcAdmin(LPVOID lpvParam)
{
	Message adminRequest;
	DWORD cbBytesRead = 0, cbReplyBytes = 0;
	int numresp = 0;
	BOOL fSuccess = FALSE;

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	while (game.getGamePhase() == IN_PROGRESS_PHASE) {

		serverPipeAdmin = CreateNamedPipe(
			lpszPipeNameAdmin, // nome do pipe
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE,
			BUFSIZE,
			5000,
			NULL);

		if (serverPipeAdmin == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("\nFalhou a criacao do pipe, erro = %d"), GetLastError());
			return 1;
		}


		fConnected = ConnectNamedPipe(serverPipeAdmin, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) {

			ReadReady = CreateEvent(
				NULL,	// default 
				TRUE,
				FALSE,
				NULL);

			if (ReadReady == NULL) {
				_tprintf(TEXT("\nServidor: não foi possível criar o evento Read. Mais vale parar já"));
				return 1;
			}


			while (threadSharedMemFlag) {

				ZeroMemory(&OverlRd, sizeof(OverlRd));
				ResetEvent(ReadReady);
				OverlRd.hEvent = ReadReady;

				fSuccess = ReadFile(
					serverPipeAdmin,
					&adminRequest,
					msg_sz,
					&cbBytesRead,
					&OverlRd);

				WaitForSingleObject(ReadReady, INFINITE);


				GetOverlappedResult(serverPipeAdmin, &OverlRd, &cbBytesRead, FALSE);

				if (cbBytesRead < msg_sz) {
					//nao leu tudo do readFile
					_tprintf(TEXT("\nErro na leitura do pipe, erro = %d"), GetLastError());
					break;
				}
				else {
					vector<string> command = getCommand(adminRequest.msg);


					switch (commandParser(command, adminRequest))
					{
					case SEED_OBJECT:
						treatCommand(command, adminRequest);
						adminRequest.code = SUCCESS;
						sprintf(adminRequest.msg, "Blocos adicionados com sucesso");
						Write(serverPipeAdmin, adminRequest);
						break;
					case FAIL:
						adminRequest.code = FAIL;
						sprintf(adminRequest.msg, "Erro no comando!");
						Write(serverPipeAdmin, adminRequest);
						break;
					default:
						break;
					}


				}
			}//end while
			FlushFileBuffers(serverPipeAdmin);
			DisconnectNamedPipe(serverPipeAdmin);
			CloseHandle(serverPipeAdmin);

		}
		else {
			CloseHandle(serverPipe);
		}
	}

	return 0;

}

