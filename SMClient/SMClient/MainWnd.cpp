#include "MainWnd.h"

#define msg_sz sizeof(Message)

#define IP "192.168.0.102"
#define LOGIN "Mário Costa"
#define PASSWORD "b432A09b1F"
#define PIPENAME "\\\\192.168.0.102\\pipe\\pipeexemplo"

bool WWindow::started = false;
tstring WWindow::AppName;
HINSTANCE WWindow::hInstance = NULL;
int idx_row = 0;
Message msg;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");
HANDLE hPipe;

HWND mainhWnd;
UINT mainmessg;
WPARAM mainwParam;
LPARAM mainlParam;

HANDLE hThread;
DWORD dwThreadId = 0;

HANDLE hThreadClient;
DWORD dwThreadClient;

HANDLE eWriteToServer;

int DeveContinuar = 1;
int ReaderAlive = 0;
int WriterAlive = 0;

HANDLE hThreadSharedMemory;
HANDLE hThreadSharedMemoryReader;
DWORD dwThreadSMReader = 0;
HANDLE hThreadSharedMemoryWriter;
DWORD dwThreadSMWriter = 0;

bool threadSharedMemFlag = true;
bool threadReadFromSMFlag = false;
bool threadWriteFromSMFlag = false;
DWORD smThreadID;



//------------Auxiliary Functions-----------------------------------------

bool LoadAndBlitBitmap(LPCSTR szFileName, HDC hWinDC, int posX, int posY)
{
	// Load the bitmap image file
	HBITMAP hBitmap;
	hBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, BITMAP_PIX_SIZE, BITMAP_PIX_SIZE,LR_LOADFROMFILE);


	// Verify that the image was loaded
	if (hBitmap == NULL) {
		MessageBox(NULL, ("LoadImage  %s Failed",szFileName), "Error", MB_OK);
		return false;
	}

	// Create a device context that is compatible with the window
	HDC hLocalDC;
	hLocalDC = CreateCompatibleDC(hWinDC);

	// Verify that the device context was created
	if (hLocalDC == NULL) {
		MessageBox(NULL, "CreateCompatibleDC Failed", "Error", MB_OK);
		return false;
	}

	// Get the bitmap's parameters and verify the get
	BITMAP qBitmap;
	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),reinterpret_cast<LPVOID>(&qBitmap));
	if (!iReturn) {
		MessageBox(NULL, "GetObject Failed", "Error", MB_OK);
		return false;
	}


	// Select the loaded bitmap into the device context
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
	if (hOldBmp == NULL) {
		MessageBox(NULL, "SelectObject Failed", "Error", MB_OK);
		return false;
	}

	

	// Blit the dc which holds the bitmap onto the window's dc
	BOOL qRetBlit = BitBlt(hWinDC, posX, posY, qBitmap.bmWidth, qBitmap.bmHeight,
		hLocalDC, 0, 0, SRCCOPY);
	if (!qRetBlit) {
		MessageBox(NULL, "Blit Failed", "Error", MB_OK);
		return false;
	}


	// Unitialize and deallocate resources
	SelectObject(hLocalDC, hOldBmp);
	DeleteDC(hLocalDC);
	DeleteObject(hBitmap);
	return true;
}


//------------Treat Responses Functions-----------------------------------------

void WWindow::treatCommand(vector<string> command, Message msg)
{
	switch (msg.code) {
		
	case SUCCESS:
		MessageBox(NULL, msg.msg, "Sucess", MB_OK);
		break;
	case ERROR:
		MessageBox(NULL, msg.msg, "Error", MB_OK);
		break;

	case CREATEGAME:
	case JOIN:
		MessageBox(NULL, msg.msg, "Nice", MB_OK);
		//Abrir janela do jogo
		DialogBox(NULL, MAKEINTRESOURCE(IDD_JOGO_PREP), HWND(), (DLGPROC)TreatDialogStartGame);
		break;
	case MAP:
		//Update ao map
		DesenhaMapa(mainhWnd, mainmessg, mainwParam, mainlParam, msg.map);
		break;

	case START:
		//dar start ao jogo
		break;

	case END:
		//Reset client
		break;

	case DISCONNECT:
		//desligar tudo
		break;
	}
}

string WWindow::commandToUpperCase(string command)
{
	for (unsigned int i = 0; i < command.size(); i++) {
		if (command[i] >= 'a' && command[i] <= 'z') {
			command[i] -= 32;
		}
	}
	return command;
}

vector<string> WWindow::getCommand(char* buffer)
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

//------------Treat Responses Functions END-------------------------------------




//------------Threads Shared Memory-----------------------------------------
DWORD WINAPI WWindow::readFromSharedMemory(LPVOID lParam) {

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

DWORD WINAPI WWindow::WriteForSharedMemory(LPVOID lParam) {


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

		if (ptr(msg)) {
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

DWORD WINAPI WWindow::ThreadSharedMemoryReader(LPVOID lParam) {

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

	while (threadSharedMemFlag) {
		//Ler da memoria partilhada e imprimir no ecra

	}

	threadReadFromSMFlag = true;
	threadSharedMemFlag = true;

	tcout << "ThreadSharedMemoryReader Closed" << endl;
	return 0;
}

//------------Threads Shared Memory END-----------------------------------------


//------------Threads Client-----------------------------------------------------

DWORD WINAPI WWindow::ThreadClientReader(LPVOID lpvParam) {
	Message response;

	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a informaçºao enviada é o handle

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	if (hPipe == NULL) {
		_tprintf(TEXT("\nThreadReader - o handle recibo no param da thread é nulo\n"));
		return -1;
	}

	ReadReady = CreateEvent(
		NULL, // default security
		TRUE,	// reset manual, por requisito do overlapped IO
		FALSE,	// estado inicial = not signaled
		NULL);	// nao precisa de nome. Uso interno ao processo

	if (ReadReady == NULL) {
		_tprintf(TEXT("\nCliente: nao foi possível criar o Evento Read. Mais vale parar já"));
		return 1;
	}

	// Ciclo de diálogo com o cliente

	while (DeveContinuar) {

		ZeroMemory(&OverlRd, sizeof(OverlRd));
		OverlRd.hEvent = ReadReady;
		ResetEvent(ReadReady);

		fSuccess = ReadFile(
			hPipe,
			&response,
			msg_sz,
			&cbBytesRead,
			&OverlRd);
			
		
		GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);
		//if (fSuccess) {
			vector<string> command = getCommand(response.msg);
			treatCommand(command, response);
		//}
			

		WaitForSingleObject(ReadReady, INFINITE);
		_tprintf(TEXT("\nRead concluido"));


		// Testar se correu como esperado

		

		if (cbBytesRead < msg_sz)
			_tprintf(TEXT("\nReadFile falhou. Erro = %d"), GetLastError());

		msg = response;
		_tprintf(TEXT("\nVeio isto do server -> %s"), msg.msg);

	

		
		// Isto so le servidor + processa mensagem. Nao escreve no pipe
		// Esse envio e feito na thread principal

	}

	ReaderAlive = 0;

	// Esta thread nao fecha o pipe.
	// O "resto do cliente" faz isso
	_tprintf(TEXT("Thread Reader a terminar. \n"));
	return 1;

}

DWORD WINAPI WWindow::ThreadClientWriter(LPVOID lpvParam) {
	//Message FromServer;
	DWORD cbWritten;
	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a informaçºao enviada é o handle

	HANDLE WriteReady; // Handle para o evento da leitura (cada thread tem um)
	OVERLAPPED OverlWr = { 0 };

	WriteReady = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (WriteReady == NULL) {
		_tprintf(TEXT("\nCliente: não foi possível criar o Evento. Mais vale parar já"));
		return 1;
	}

	_tprintf(TEXT("\nligação estabelecida. \"exit\" para sair"));

	while (DeveContinuar) {

		WaitForSingleObject(eWriteToServer, INFINITE);

		ZeroMemory(&OverlWr, sizeof(OverlWr));
		ResetEvent(WriteReady);
		OverlWr.hEvent = WriteReady;

		fSuccess = WriteFile(
			hPipe,
			&msg,
			msg_sz,
			&cbWritten,
			&OverlWr);

		WaitForSingleObject(WriteReady, INFINITE);
		_tprintf(TEXT("\nWrite concluido"));

		GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);

		if (cbWritten < msg_sz)
			_tprintf(TEXT("\nWriteFile TALVEZ falhou. Erro = %d"), GetLastError());

		_tprintf(TEXT("\nMessagem enviada"));
		ResetEvent(eWriteToServer);

	}
	_tprintf(TEXT("\nEncerrar a thread ouvinte"));

	DeveContinuar = 0;

	_tprintf(TEXT("\nCleinte vai terminar ligação e sair"));
	CloseHandle(WriteReady);
	CloseHandle(hPipe);

	WriterAlive = 0;
	_tprintf(TEXT("Thread Writer a terminar. \n"));
	return 1;
}

DWORD WINAPI WWindow::ThreadConnectClient(LPVOID lpvParam) {
	DWORD dwMode;
	BOOL fSuccess = false;
	DWORD cbWritten;

	HANDLE hUserToken = NULL;
	BOOL log;

	TCHAR username[20],	// username da máquina destino
		pass[20],			// password desse utilizador (na máquina destino)
		dominio[20];		// pode ser o IP da máquina


	_tcscpy(dominio, TEXT(IP));
	_tcscpy(username, TEXT(LOGIN));
	_tcscpy(pass, TEXT(PASSWORD));

	//_tcscpy(lpszPipename, TEXT("\\\\"));
	//_tcscat(lpszPipename, dominio);
	lpszPipename = TEXT(PIPENAME);

	// porque nao retorna um BOOL???
	log = LogonUser(username,
		dominio,
		pass,
		LOGON32_LOGON_NEW_CREDENTIALS,	// tipo de logon
		LOGON32_PROVIDER_DEFAULT,		// logon provider
		&hUserToken);

	//MessageBox(NULL, TEXT("Numero de Jogadores errado [1,10]"), TEXT("Erro"), MB_ICONERROR);

	log = ImpersonateLoggedOnUser(hUserToken);

	while (1) {

		hPipe = CreateFile(
			lpszPipename, // Nome do pipe remoto
			GENERIC_READ | // acesso read e write
			GENERIC_WRITE,
			0 | FILE_SHARE_READ | FILE_SHARE_WRITE, // sem -> com partilha
			NULL,
			OPEN_EXISTING,
			0 | FILE_FLAG_OVERLAPPED,
			NULL);

		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() != ERROR_PIPE_BUSY) {
			// Nao ha nenhum servidor a correr
			
			MessageBox(NULL, "Nenhum servidor disponivel de momento!", "Erro!", MB_OK);

			DWORD i = GetLastError();
			_tprintf(TEXT("\nCreate file deu erro e nao foi BUSY. Erro = %d\n"), GetLastError());
			return -1;

			
		}

		// Se chegou aqui é porque todas as instªancias
		// do pipe estão ocupadas. Remédio: aguardar que uma
		// fique livre com um timeout

		if (!WaitNamedPipe(lpszPipename, 30000)) {
			_tprintf(TEXT("Esperei por uma instancia durante 30 segundos"));
			return -1;

		}

	}

	// Aqui é que se tem de abrir a dialog box!!!

	dwMode = PIPE_READMODE_MESSAGE;
	fSuccess = SetNamedPipeHandleState(
		hPipe,		// handle para o pipe
		&dwMode,	// Novo modo do pipe
		NULL,		// Nao é para mudar max. bytes
		NULL);		// Nao e para mudar max. timeout

	if (!fSuccess) {
		_tprintf(TEXT("SetNamedPipeHandleState falhou. Erro = %d\n"), GetLastError());
		return -1;
	}

	//Envia a primeira mensagem
	HANDLE WriteReady; // Handle para o evento da leitura (cada thread tem um)
	OVERLAPPED OverlWr = { 0 };

	WriteReady = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (WriteReady == NULL) {
		_tprintf(TEXT("\nCliente: não foi possível criar o Evento. Mais vale parar já"));
		return 1;
	}

	_tprintf(TEXT("\nligação estabelecida. \"exit\" para sair"));

	msg.pid = GetCurrentThreadId();

	ZeroMemory(&OverlWr, sizeof(OverlWr));
	ResetEvent(WriteReady);
	OverlWr.hEvent = WriteReady;

	// Escreve no pipe
	fSuccess = WriteFile(
		hPipe,
		&msg,
		msg_sz,
		&cbWritten,
		&OverlWr);

	WaitForSingleObject(WriteReady, INFINITE);
	_tprintf(TEXT("\nWrite concluido"));

	GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);

	if (cbWritten < msg_sz)
		_tprintf(TEXT("\nWriteFile TALVEZ falhou. Erro = %d"), GetLastError());

	CloseHandle(WriteReady);

	// Create READ Thread
	hThread = CreateThread(
		NULL,
		0,
		ThreadClientReader,
		(LPVOID)hPipe,
		0,
		&dwThreadId);

	if (hThread == NULL) {
		_tprintf(TEXT("\nErro na criação da thread. Erro = %d"), GetLastError());
		return -1;
	}

	// Create WRITE Thread
	hThread = CreateThread(
		NULL,
		0,
		ThreadClientWriter,
		(LPVOID)hPipe,
		0,
		&dwThreadId);

	if (hThread == NULL) {
		_tprintf(TEXT("\nErro na criação da thread. Erro = %d"), GetLastError());
		return -1;
	}
	return 1;
}

//------------Thread Client END-------------------------------------------------


LRESULT CALLBACK WWindow::DesenhaMapa(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	Map map)
{

	PAINTSTRUCT ps;
	HDC hdc;

	int x = map.actualX;
	int y = map.actualY;

	x = x * BITMAP_PIX_SIZE;
	y = y * BITMAP_PIX_SIZE;

	int ii = 0;
	int jj = 0;


	switch (message) {
	case WM_CREATE:

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN: // Programacao das teclas
	{
		switch (wParam)
		{
		case VK_LEFT:
			//MessageBox(hwnd, "LEFT Arrow", "Key Pressed", MB_OK);
			break;
		case VK_RIGHT:
			//MessageBox(hwnd, "RIGHT Arrow", "Key Pressed", MB_OK);
			break;
		case VK_UP:
			//MessageBox(hwnd, "UP Arrow", "Key Pressed", MB_OK);
			break;
		case VK_DOWN:
			//MessageBox(hwnd, "DOWN Arrow", "Key Pressed", MB_OK);
			break;
		default:
			MessageBox(hwnd, "UNKONWN key pressed!", "Key Pressed", MB_OK);
			break;
		}
	}
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		// PAREDES - INICIO
		LoadAndBlitBitmap("skblock.bmp", hdc, 0, 0); // canto superior esquerdo
		for (int i = BITMAP_PIX_SIZE; i < x-1; i += BITMAP_PIX_SIZE)
			LoadAndBlitBitmap("skblock.bmp", hdc, i, 0); // parede superior	
		LoadAndBlitBitmap("skblock.bmp", hdc, x, 0); // canto superior direito

		for (int i = BITMAP_PIX_SIZE; i < y - 1; i += BITMAP_PIX_SIZE) {
			LoadAndBlitBitmap("skblock.bmp", hdc, 0, i); // parede lateral esquerda
			LoadAndBlitBitmap("skblock.bmp", hdc, x, i); // parede lateral direita
		}

		LoadAndBlitBitmap("skblock.bmp", hdc, 0, y); // canto inferior esquerdo
		for (int i = BITMAP_PIX_SIZE; i < x - 1; i += BITMAP_PIX_SIZE)
			LoadAndBlitBitmap("skblock.bmp", hdc, i, y); // parede inferior
		LoadAndBlitBitmap("skblock.bmp", hdc, x, y); // canto inferior direito
		// PAREDES - FIM

		for (int i = 0; i < map.actualX; i++) {

			if (i == map.actualX-1)
				ii = 0;
			ii += BITMAP_PIX_SIZE;
			

			for (int j = 1; j < map.actualY; j++) {

				if (j == map.actualY - 1)
					jj = 0;
				jj += BITMAP_PIX_SIZE;
				

				switch (map.map[i][j]){
					case '_': // grass
						LoadAndBlitBitmap("field.bmp", hdc, ii, jj);
						break;
					case 'i': // ice
						LoadAndBlitBitmap("ice.bmp", hdc, ii, jj);
						break;
					case 'f': // food
						LoadAndBlitBitmap("food.bmp", hdc, ii, jj);
						break;
					case 'v': // vodka
						LoadAndBlitBitmap("vodka.bmp", hdc, ii, jj);
						break;
					case 'b': // granade
						LoadAndBlitBitmap("granade.bmp", hdc, ii, jj);
						break;
					case 'o': // oil
						LoadAndBlitBitmap("oil.bmp", hdc, ii, jj);
						break;
					case 'g': // glue
						LoadAndBlitBitmap("glue.bmp", hdc, ii, jj);
						break;
					case 'V': // vodka
						LoadAndBlitBitmap("o_vodka.bmp", hdc, ii, jj);
						break;
					case 'O': // oil
						LoadAndBlitBitmap("o_oil.bmp", hdc, ii, jj);
						break;
					case 'G': // glue
						LoadAndBlitBitmap("o_glue.bmp", hdc, ii, jj);
						break;
					case 's': // snake
						LoadAndBlitBitmap("snake.bmp", hdc, ii, jj);
						break;
					case '>': // oiled snake block
						LoadAndBlitBitmap("skblock.bmp", hdc, ii, jj);
						break;
					case '<': // glued snake block
						LoadAndBlitBitmap("snake_glue.bmp", hdc, ii, jj);
						break;
					case '+': // vodka
						LoadAndBlitBitmap("snake_vodka.bmp", hdc, ii, jj);;
						break;
					case 'c': // coffe
						LoadAndBlitBitmap("skblock.bmp", hdc, ii, jj);;
						break;
					default: // grass
						LoadAndBlitBitmap("field.bmp", hdc, ii, jj);
						break;
				}
			}
		}

		EndPaint(hwnd, &ps);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}


	return 0;


}


WWindow::WWindow(
	LPCTSTR clsname, 
	LPCTSTR wndname,
	HWND parent,
	DWORD dStyle,
	DWORD dXStyle,
	int x,
	int y,
	int width,
	int height)
{

	AppName = clsname;
	//Registar a classe se ainda não foi registada antes
	if (!started)
		StartUp();

	// Criar uma nova janela
	_hwnd = CreateWindowEx(dXStyle, clsname, wndname, dStyle, x, y, width,
		height, parent, NULL, hInstance, NULL);

	// Se a janela não foi criada terminar o programa!
	if (_hwnd == NULL) {
		MessageBox(NULL, TEXT("na criacao da janela"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		exit(1);
	}
	//Armazenar o ponteiro this na zona cbClsExtra da estrutura WNDCLASSEX
	SetWindowLongPtr(_hwnd, 0, (long) this);

	eWriteToServer = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("WriteToServerEvent")  // object name
	);


	if (eWriteToServer == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return;
	}
}


	


//---------------------------------------------------------------------------
bool WWindow::Register() {
	WNDCLASSEX _WndClsEx;
	//Definição das características da janela "_WndClsEx"
	_WndClsEx.cbSize = sizeof(WNDCLASSEX);
	_WndClsEx.style = CS_HREDRAW | CS_VREDRAW;
	_WndClsEx.lpfnWndProc = WndProc;	//Função Membro
	_WndClsEx.cbClsExtra = 0;
	_WndClsEx.cbWndExtra = sizeof(WWindow *);
	_WndClsEx.hInstance = GetModuleHandle(AppName.c_str());
	_WndClsEx.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
	_WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WndClsEx.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	_WndClsEx.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	_WndClsEx.lpszClassName = AppName.c_str();
	_WndClsEx.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
	if (!RegisterClassEx(&_WndClsEx)) {
		MessageBox(NULL, TEXT("no registo da classe"), TEXT("Erro"), MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}


//---------------------------------------------------------------------------
BOOL WWindow::Show(int dCmdShow)
{
	// ============================================================================
	// Mostrar a janela
	// ============================================================================
	if (!ShowWindow(_hwnd, dCmdShow))	// "hWnd"= handler da janela, devolvido 
		return FALSE;					// por "CreateWindow"; "dCmdShow"= modo de
										// exibição (p.e. normal, modal); é passado
										// como parâmetro de WinMain()

	if (!UpdateWindow(_hwnd))			// Refrescar a janela (Windows envia à janela
		return FALSE;					// uma mensagem para pintar, mostrar dados,
										// (refrescar), etc)

	return TRUE;
}
//---------------------------------------------------------------------------
//Confirmar fecho da janela
bool WWindow::doConfirmation(void) {
	if (MessageBox(_hwnd, TEXT("Tem certeza?"), TEXT("Sair"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		return true;
	return false;
}
//---------------------------------------------------------------------------
/*WWindow::operator HWND()
{
	//Uma vez que cada janela é do tipo HWND, utilizaremos 
	//um modo de reconhecer o handle da janela quando utilizado 
	//na aplicação
	return _hwnd;
}*/
//---------------------------------------------------------------------------
void WWindow::StartUp(void) {
	hInstance = GetModuleHandle(AppName.c_str());

	if (Register())
		started = true;

}
//---------------------------------------------------------------------------
LRESULT WWindow::WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
	//Saber sobre que janela estamos a trabalhar
	HDC hdc;
	PAINTSTRUCT PtStc;
	WWindow *pWin = (WWindow *)GetWindowLongPtr(hWnd, 0);
	BOOL eRato = FALSE;

	mainhWnd = hWnd;
	mainmessg = messg;
	mainwParam = wParam;
	mainlParam = lParam;

	//TCHAR erro[100];
	//DWORD lastError;

	/*Map map;

	map.actualX = 10;
	map.actualY = 10;

	for (int i = 0; i < map.actualX; i++) {
		for (int j = 0; j < map.actualY; j++) {
			map.map[i][j] = '_';
		}
	}

	map.map[5][5] = 's';

	map.map[7][7] = 'V';

	map.map[2][7] = 'g';

	map.map[1][1] = 'o';*/

	


	int direction;
	switch (messg)
	{
	case WM_CLOSE:
		threadSharedMemFlag = false;
		if (hPipe != NULL && hPipe != INVALID_HANDLE_VALUE) {
			msg.code = DISCONNECT;
			sprintf(msg.msg, "");
			SetEvent(eWriteToServer);
			DeveContinuar = 0;
		}
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
			//Menu		
		case ID_JOGO_LIGARREMOTAMENTE: // Create Instance of "ThreadConnectClient" for multiplayer game

			CreateThread(
				NULL,
				0,
				ThreadConnectClient,
				(LPVOID)hThreadClient,
				0,
				&dwThreadClient);
			DialogBox(NULL, MAKEINTRESOURCE(IDD_LIGAR_SERVIDOR), hWnd, (DLGPROC)TreatDialogConnectToServer);


			//lastError = GetLastError();
			//_stprintf_s(erro, _T("%X"), lastError);
			 
			//MessageBox(hWnd, erro, "Erro", MB_OK);

			

			//EnableWindow(GetDlgItem(hWnd, ID_CRIAR_JOGO), FALSE); // isto nao esta a funcionar nao sei pq
																  // deveria desativar o botao

			break;
		case ID_JOGO_LIGARLOCALMENTE:
			CreateThread(
				NULL,
				0,
				ThreadSharedMemoryReader,
				(LPVOID)hThreadSharedMemory,
				0,
				&smThreadID);

			DialogBox(NULL, MAKEINTRESOURCE(IDD_LIGAR_SERVIDOR), hWnd, (DLGPROC)TreatDialogConnectToServer);
			//threads da dll



			break;
		case ID_EDITAR_EDITARSKINS:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_EDITAR_IMAGENS), hWnd, (DLGPROC)TreatDialogEditSkins);
			break;
		case ID_JOGO_SAIR:
			PostQuitMessage(0);
			break;
		case ID_SOBRE:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)TreatDialogJoinGame);
			break;

			//Keyboard
		case WM_KEYDOWN:
			if (wParam == VK_LEFT)
				direction = GOING_LEFT;
			if (wParam == VK_RIGHT)
				direction = GOING_RIGHT;
			if (wParam == VK_UP)
				direction = GOING_UP;
			if (wParam == VK_DOWN)
				direction = GOING_DOWN;

			//enviar a direção
			msg.code = SETDIRECTION;
			sprintf(msg.msg, "%d", direction);
			SetEvent(eWriteToServer);

			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &PtStc);
			break;
		default:
			return FALSE;
		}
	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam));
		break;
	}

	return(0);
}

LRESULT CALLBACK WWindow::TreatDialogCreateGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	int numPlayers;
	int numObjects;
	int width;
	int height;
	int initialSnakeTam;
	int numSnakesAI;
	TCHAR playerName[256];

	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_DIALOG_CREATE_BUTTON:
			GetWindowText(GetDlgItem(hWnd, IDC_EDIT_PLAYER_NAME), playerName, 256);
			numPlayers = GetDlgItemInt(hWnd, IDC_EDIT_NUM_PLAYERS, NULL, FALSE);
			if (numPlayers < 0 || numPlayers > 10) {
				MessageBox(hWnd, TEXT("Numero de Jogadores errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			numObjects = GetDlgItemInt(hWnd, IDC_EDIT_NUM_OBJECTS, NULL, FALSE);
			if (numObjects <= 0) {
				MessageBox(hWnd, TEXT("Numero de Objetos errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			width = GetDlgItemInt(hWnd, IDC_EDIT_MAP_WIDTH, NULL, FALSE);
			if (width < 0 || width > 80) {
				MessageBox(hWnd, TEXT("Comprimento errado [1,80]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			height = GetDlgItemInt(hWnd, IDC_EDIT_MAP_HEIGHT, NULL, FALSE);
			if (height < 0 || height > 80) {
				MessageBox(hWnd, TEXT("Largura errada [1,80]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			initialSnakeTam = GetDlgItemInt(hWnd, IDC_EDIT_SNAKE_SIZE, NULL, FALSE);
			if (initialSnakeTam < 0 || initialSnakeTam > 10) {
				MessageBox(hWnd, TEXT("Tamanho inicial das cobras errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}
			numSnakesAI = GetDlgItemInt(hWnd, IDC_EDIT_NUM_SNAKES_AI, NULL, FALSE);
			if (numSnakesAI < 0 || numSnakesAI > 10) {
				MessageBox(hWnd, TEXT("Numero de Cobras AI errado [1,10]"), TEXT("Erro"), MB_ICONERROR);
				return FALSE;
			}

			//Enviar o comando aqui
			msg.code = CREATEGAME;
			sprintf(msg.msg, "%d %d %d %d %d %d %s", width, height, numPlayers, initialSnakeTam, numObjects, numSnakesAI, playerName);
			SetEvent(eWriteToServer);
			EndDialog(hWnd, 0);
			return TRUE;
		case ID_DIALOG_CREATE_CANCEL:
			EndDialog(hWnd, TRUE);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}

LRESULT CALLBACK WWindow::TreatDialogJoinGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	TCHAR playerName[256];

	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_DLG_JOIN_OK:
			GetWindowText(GetDlgItem(hWnd, ID_DLG_JOIN_PLAYER_NAME), playerName, 256);
			msg.code = JOIN;
			sprintf(msg.msg, "JOIN %s", playerName);
			SetEvent(eWriteToServer);
			
			DialogBox(NULL, MAKEINTRESOURCE(IDD_JOGO_PREP), hWnd, (DLGPROC)TreatDialogStartGame);
			//EndDialog(hWnd, 0);
			return TRUE;
		case ID_DLG_JOIN_CANCEL:
			EndDialog(hWnd, TRUE);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}

LRESULT CALLBACK WWindow::TreatDialogConnectToServer(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {


	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_CRIAR_JOGO:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_CREATE_GAME), hWnd, (DLGPROC)TreatDialogCreateGame);
			EndDialog(hWnd, 0);
			return TRUE;
		case ID_JUNTAR_AO_JOGO:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG_JOIN), hWnd, (DLGPROC)TreatDialogJoinGame);
			EndDialog(hWnd, 0);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}

LRESULT WWindow::TreatDialogEditSkins(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HWND hCombo = GetDlgItem(hWnd, IDC_COMBO1);
	TCHAR skins[14][20] = {
		TEXT("Cobra"),
		TEXT("Cobra com Gelo"),
		TEXT("Cobra com Cola"),
		TEXT("Cobra com Vodka"),
		TEXT("Cola"),
		TEXT("Cola Especial"),
		TEXT("Comida"),
		TEXT("Gelo"),
		TEXT("Granada"),
		TEXT("Oleo"),
		TEXT("Oleo Especial"),
		TEXT("Vodka"),
		TEXT("Vodka Especial"),
		TEXT("Terreno")
	};

	TCHAR A[16];
	int  k = 0;
	TCHAR cmd[256] = "mspaint.exe ..\\\\";
	switch (messg) {
	case WM_INITDIALOG:
		// preenche a combobox.
		memset(&A, 0, sizeof(A));
		for (k = 0; k <= 13; k += 1) {
			strcpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)skins[k]);
			SendMessage(hCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)A);
		}
		SendMessage(hCombo, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;

	case WM_COMMAND:


		switch (LOWORD(wParam)) {
		case IDC_COMBO1:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				idx_row = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			}
			return TRUE;

		case ID_ESCOLHA_SKIN:
			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			//idx_row = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			//SendMessage(hCombo, CB_GETLBTEXT, idx_row, (LPARAM)strText);

			switch (idx_row) {
			case 0:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "snake.bmp");
				break;
			case 1:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "snake_ice.bmp");
				break;
			case 2:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "snake_glue.bmp");
				break;
			case 3:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "snake_vodka.bmp");
				break;
			case 4:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "glue.bmp");
				break;
			case 5:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "o_glue.bmp");
				break;
			case 6:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "food.bmp");
				break;
			case 7:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "ice.bmp");
				break;
			case 8:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "granade.bmp");
				break;
			case 9:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "oil.bmp");
				break;
			case 10:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "o_oil.bmp");
				break;
			case 11:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "vodka.bmp");
				break;
			case 12:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "o_vodka.bmp");
				break;
			case 13:
				strcat_s(cmd, sizeof(cmd) / sizeof(TCHAR), "field.bmp");
				break;
			default:
				break;
			}

			// Start the child process. 
			if (!CreateProcess(
				NULL,   // No module name (use command line)
				cmd,        // Command line
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			{
				printf("CreateProcess failed (%d).\n", GetLastError());
				return FALSE;
			}

			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles. 
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			EndDialog(hWnd, 0);

			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
	return 0;
}

LRESULT CALLBACK WWindow::TreatDialogStartGame(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	switch (messg) {
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDD_START_GAME:
			msg.code = START;
			sprintf(msg.msg, "");
			SetEvent(eWriteToServer);
			EndDialog(hWnd, 0);
			return TRUE;
		case IDD_LEAVE:
			msg.code = DISCONNECT;
			sprintf(msg.msg, "");
			SetEvent(eWriteToServer);
			EndDialog(hWnd, TRUE);
			return TRUE;
		default:
			return FALSE;
		}
		return TRUE;
	default:
		return FALSE;
	}
}
