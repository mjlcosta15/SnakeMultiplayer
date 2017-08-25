#include "Server.h"

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

LPCTSTR lpName;
DWORD dwOpenMode;
DWORD dwPipeMode;
DWORD nMaxInstances;
DWORD nOutBufferSize;
DWORD nInBufferSize;
DWORD nDefaultTimeOut;
LPSECURITY_ATTRIBUTES lpSecurityAttributes;

LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");

HANDLE WriteReady;

// Client's Array HANDLES
HANDLE clients[MAX_PLAYERS];

HANDLE hThread;

BOOL fConnected = FALSE;
DWORD dwThreadId = 0;

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


DWORD WINAPI readFromSharedMemory(LPVOID lParam) {

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
		if(msg != nullptr)
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

	while (1) {

		if (ptr(game.exportInfoToMessage())) {
			//tcout << TEXT("ENviado com sucesso") << endl;
		}
		else {
			//tcout << TEXT("Erro ao enviar") << endl;
		}

		if (threadWriteFromSMFlag) {
			return 1;
		}

	}

	return 1;
}

unsigned int __stdcall ThreadSharedMemoryReader(void * p)
{
	Server * server = (Server *)p;

	CreateThread(
		NULL,
		0,
		readFromSharedMemory,
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


void Server::startServer()
{

	if (!smHelper.initSharedMemory()) {
		finishServer();
	}

	//lançar a thread the leitura
	threadSharedMemFlag = true;
	_beginthreadex(0, 0, ThreadSharedMemoryReader, this, 0, &smThreadID);
	hThreadSharedMemory = OpenThread(THREAD_ALL_ACCESS, FALSE, smThreadID);

	waitConnection();

	//serverMainLoop();
}

void Server::serverMainLoop()
{

	tcout << "Server Online." << endl;
	game.setInitalPhase();
	//iniciar thread de aceitar clientes
	do {

		//Initial Phase
		if (game.getGamePhase() == INITIAL_PHASE)
			initialPhaseLoop();
		//Game Phase
		if (game.getGamePhase() == IN_PROGRESS_PHASE) {
			//fechar thread de aceitar clientes
			GamePhaseLoop();
		}


	} while (game.getGamePhase() == FINISH_PHASE);
	//Finish Phase
	finishServer();
}

void Server::initialPhaseLoop()
{
	tcout << "Initial Phase Loop started" << endl;

	do {

		game.setMapHeight(10);
		game.setMapWidth(10);
		game.setNumSnakesAI(3);
		game.setNumberOfObjects(3);
		game.setSnakeSize(3);
		game.addPlayer(1, "jorge");
		game.initMap();
		//		game.setInProgressPhase();

	} while (game.getGamePhase() == INITIAL_PHASE);
}

void Server::GamePhaseLoop()
{
	tcout << "Game Phase Loop started" << endl;
	do {
		game.updateMap();
		Sleep(33); //Fazer 30 atualizações por segundo (30 FPS oh yeah)
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
}


bool Server::commandParser(vector<string> command)
{
	if (!command.size())
		return false;

	//START
	if (command[0] == "START") {
		if (!game.getGamePhase() == INITIAL_PHASE) {
			return true;
		}
		return false;
	}

	//CREATEGAME <Map Width [1,80]> <Map Height [1,80]> <Nr Players [1,10]> <Snake Size [1,10]> <Nr Objects [1,10]> <Nr Snakes AI [1,10]> <Player Username>
	else if (command[0] == "CREATEGAME") {
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 8) {
				if (stoi(command[1]) >= 1 && stoi(command[1]) <= MAX_TAM_MAP) {
					if (stoi(command[2]) >= 1 && stoi(command[2]) <= MAX_TAM_MAP) {
						if (stoi(command[3]) >= 1 && stoi(command[3]) <= MAX_PLAYERS) {
							if (stoi(command[4]) >= 1 && stoi(command[4]) <= MAX_PLAYERS) {
								if (stoi(command[5]) >= 1 && stoi(command[5]) <= MAX_PLAYERS) {
									if (stoi(command[6]) >= 1 && stoi(command[6]) <= MAX_PLAYERS) {
										if (command[7] != "") {
											return true;
										}
										else {
											cout << "COMMAND -" << command[0] << "- Error in Username" << endl;

										}
									}
									else {
										cout << "COMMAND -" << command[0] << "- Error in Nr Snakes AI" << endl;

									}
								}
								else {
									cout << "COMMAND -" << command[0] << "- Error in Nr Objects" << endl;
								}
							}
							else {
								cout << "COMMAND -" << command[0] << "- Error in Snake Size" << endl;
							}
						}
						else {
							cout << "COMMAND -" << command[0] << "- Error in Nr Players" << endl;
						}
					}
					else {
						cout << "COMMAND -" << command[0] << "- Error in Map Height" << endl;
					}
				}
				else {
					cout << "COMMAND -" << command[0] << "- Error in Map Width" << endl;
				}
				return false;
			}
			return false;
		}
		return false;
	}//JOIN <username>
	else if (command[0] == "JOIN") {
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 2)
				if (command[1] != "")
					return true;
				else
					cout << "COMMAND -" << command[0] << "- Error in Username" << endl;
			return false;
		}
		return false;
	}
	//SETDIRECTION <Direction(1,2,3,4)>
	else if (command[0] == "SETDIRECTION") {
		if (command.size() == 2) {
			if (game.getGamePhase() == IN_PROGRESS_PHASE)
				if (stoi(command[1]) > 0 && stoi(command[1]) < GOING_RIGHT)
					return true;
				else
					cout << "COMMAND -" << command[0] << "- Error in Direction" << endl;
			return false;
		}
		return false;
	}//DISCONNECT <PID>
	else if (command[0] == "DISCONNECT") {
		if (command.size() == 2) {
			char* p;
			strtol(command[1].c_str(), &p, 10);
			if (*p == 0)
				return true;
			else
				cout << "COMMAND -" << command[0] << "- Error in PID" << endl;
			return false;
		}
		return false;
	}
	else {
		//WRONG COMMAND
		return false;
	}
	return false;
}


void Server::treatCommand(vector<string> command, Message msg)
{
	if (command[0] == "START") {
		game.setInProgressPhase();
	}
	else if (command[0] == "CREATEGAME") {
		game.setMapWidth(stoi(command[1]));
		game.setMapHeight(stoi(command[2]));
		game.setNumPlayers(stoi(command[3]));
		game.setSnakeSize(stoi(command[4]));
		game.setNumberOfObjects(stoi(command[5]));
		game.setNumSnakesAI(stoi(command[6]));
		game.addPlayer(new Player(msg.pid, command[7], &game));
	}
	else if (command[0] == "JOIN") {
		game.addPlayer(new Player(msg.pid, command[1], &game));
	}
	else if (command[0] == "SETDIRECTION") {
		game.setDirectionToPlayer(msg.pid, stoi(command[1]));
	}
	else if (command[0] == "DISCONNECT") {
		game.removePlayer(msg.pid);
	}

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

bool Server::getSharedMemFlag() const {
	return threadSharedMemFlag;
}


int Server::waitConnection()
{

	

	/*************Esta parte é nova***************/

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

	PSECURITY_DESCRIPTOR sd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR,
		SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(sd, TRUE, acl, FALSE);

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

	/**********************************************/

	// Enter the cicle

	while (1) {

		// Pipe Local

		serverPipe = CreateNamedPipe(
			lpszPipename, // nome do pipe
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE,
			BUFSIZE,
			5000,
			NULL);

		// Pipe Remoto

		/*
serverPipe = CreateNamedPipe(
			lpszPipename, // nome do pipe
			PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			BUFSIZE,
			BUFSIZE,
			5000,
			&sa);*/ // este argumento passa de 'NULL' para -> &sa que é um ponteiro para a estrutura security attributes

		if (serverPipe == INVALID_HANDLE_VALUE) {
			_tprintf(TEXT("\nFalhou a criacao do pipe, erro = %d"), GetLastError());
			return -1;
		}

		_tprintf(TEXT("\nServidor a aguardar que um cliente se ligue"));

		fConnected = ConnectNamedPipe(serverPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) {

			hThread = CreateThread(
				NULL,
				0,
				InstanceThread,
				(LPVOID)serverPipe,
				0,
				&dwThreadId);

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

	if (cbWritten < msg_sz)
		_tprintf(TEXT("\nNao chegou tudo %d"), GetLastError());

	return 1;

}

// Recebe o HANDLE e adiciona o cliente à lista de clientes
void Server::addClient(HANDLE cli) {

	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == NULL) {
			clients[i] = cli;
			return;
		}
	}

}

void Server::rmClient(HANDLE cli) {

	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == cli) {
			clients[i] = NULL;
			return;
			;
		}
	}
}

int Server::Broadcast(Message msg) {

	int i, numwrites = 0;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] != 0)
			numwrites += Write(clients[i], msg);
	}

	return 0;

}

void Server::setfConnected(BOOL flag) {

	fConnected = flag;

}

void Server::initializeClients()
{
	int i;
	for (i = 0; i < MAX_PLAYERS; i++)
		clients[i] = NULL;
}

void Server::setHNamedPipe(HANDLE namedPipe) {

	serverPipe = namedPipe;

}

HANDLE Server::getHNamedPipe()
{
	return HANDLE();
}

DWORD WINAPI Server::InstanceThread(LPVOID lpvParam)
{
	Message Pedido, Resposta;
	DWORD cbBytesRead = 0, cbReplyBytes = 0;
	int numresp = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a infroamacao enviada à thread é o handle do pipe

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	//_tcscpy(to_string(Resposta.pid), TEXT("SRV"))

	cout << "ola mundo" << endl;

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

	addClient(hPipe); // Regista cliente

	while (1) {

		ZeroMemory(&OverlRd, sizeof(OverlRd));
		ResetEvent(ReadReady);
		OverlRd.hEvent = ReadReady;

		fSuccess = ReadFile(
			hPipe,
			&Pedido,
			msg_sz,
			&cbBytesRead,
			&OverlRd);

		WaitForSingleObject(ReadReady, INFINITE);

		GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);

		if (cbBytesRead < msg_sz) {
			_tprintf(TEXT("\nReadFile não leu os dados todos. Erro = %d"), GetLastError());

			if (!fSuccess || cbBytesRead < msg_sz) {
				_tprintf(TEXT("\nServidor: Recebi(?) de: [%d] msg: [%s]"), Pedido.pid, Pedido.msg);
				// _tcspy(Resposta.msg, s);
				//_tcscat(Resposta.msg, Pedido.msg);
				strcat_s(Resposta.msg, Pedido.msg);
				numresp = Broadcast(Resposta);
				_tprintf(TEXT("\nServidor: %d respostas enviadas"), numresp);
			}

			rmClient(serverPipe);

			FlushFileBuffers(serverPipe);
			DisconnectNamedPipe(serverPipe);
			CloseHandle(serverPipe);

			_tprintf(TEXT("\nThread dedicada Cliente a terminar"));
			return 1;

		}

	}

}

