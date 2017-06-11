#include "Client.h"

LPCTSTR lpFileName;
DWORD dwDesiredAccess;
DWORD dwShareMode;
LPSECURITY_ATTRIBUTES lpSecurityAttributes;
DWORD dwCreaationDisposition;
DWORD dwFlaghsAndAttributes;
HANDLE hTemplateFile;

LPCSTR lpNamedPipeName;
DWORD nTimeOut;

HANDLE readReady;
OVERLAPPED OverlRd;

HANDLE hPipe;
BOOL fSuccess = false;

Message msg;
LPDWORD cbBytesRead;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");

HANDLE hThread;

DWORD WINAPI ThreadClientReader(LPVOID lpvParam);
DWORD dwThreadId = 0;


int DeveContinuar = 1;
int ReaderAlive = 0;

DWORD WINAPI ThreadClienteReader(LPVOID lpvParam) {
	Message fromServer;

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
			&FromServer,
			msg_sz,
			&cbBytesRead,
			&OverlRd);

		WaitForSingleObject(ReadReady, INFINITE);
		_tprintf(TEXT("\nRead concluido"));


		// Testar se correu como esperado

		GetOverlappedResult(hPipe, &OverlRd, &cbBytesRead, FALSE);

		if (cbBytesRead < msg_sz)
			_tprintf(TEXT("\nReadFile falhou. Erro = %d"), GetLastError());
	
		//_tprintf(TEXT("\nServidor disse: [%s]"), FromServer.msg);

		// Isto so le servidor + processa mensagem. Nao escreve no pipe
		// Esse envio e feito na thread principal

	}

	ReaderAlive = 0;

	// Esta thread nao fecha o pipe.
	// O "resto do cliente" faz isso
	_tprintf(TEXT("Thread Reader a terminar. \n"));
	return 1;

}


Client::Client()
{
	
}


Client::~Client()
{
}

int Client::start() {

	
	DWORD cbWritten, dwMode;
	

	Message msg;

	_setmode(_fileno(stdout), _O_WTEXT); // proteger com #ifdef UNICODE ... #endif

	_tprintf(TEXT("Escreve nome >"));
	//readTChars(MsgToSend.quem, QUEMSZ);

	//Create(); // liga-se ao pipe
	//InitializeOverlappedStructure();
	//Wait();

	while (1) {

		hPipe = CreateFile(
			lpszPipename, // Nome do pipe
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

	hThread = CreateThread(
		NULL,
		0,
		ThreadClienteReader,
		(LPVOID)hPipe,
		0,
		&dwThreadId);

	if (hThread == NULL) {
		_tprintf(TEXT("\nErro na criação da thread. Erro = %d"), GetLastError());
		return -1;
	}

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

	while (1) {
		//_tprintf(TEXT("\n%s > "), MsgToSend.quem);
		//readTChars(MsgToSend.msg, MSGTXSZ);

		//if (_tcscmp(TEXT("exit"), MsgToSend.msg) == 0)
			//break;

		//_tprintf(TEXT("\nA enviar %d bytes: \""%s\""), msg_sz, msgtosend.msg);

		ZeroMemory(&OverlWr, sizeof(OverlWr));
		ResetEvent(WriteReady);
		OverlWr.hEvent = WriteReady;

		fSuccess = WriteFile(
			hPipe,
			&MsgToSend,
			msg_sz,
			&cbWritten,
			&OverlWr);

		WaitForSingleObject(WriteReady, INFINITE);
		_tprintf(TEXT("\nWrite concluido"));

		GetOverlappedResult(hPipe, &OverlWr, &cbWritten, FALSE);

		if (cbWritten < msg_sz)
			_tprintf(TEXT("\nWriteFile TALVEZ falhou. Erro = %d"), GetLastError());

		_tprintf(TEXT("\nMessagem enviada"));

	}
	_tprintf(TEXT("\nEncerrar a thread ouvinte"));

	DeveContinuar = 0;

	if (ReaderAlive) {
		WaitForSingleObject(hThread, 3000);
		_tprintf(TEXT("\nCliente vai terminar ligação e sair"));

	}

	_tprintf(TEXT("\nCleinte vai terminar ligação e sair"));
	CloseHandle(WriteReady);
	CloseHandle(hPipe);
	//pressEnter();
	return 0;

	

}

void Client::OpenPipe() {

	hPipe = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreaationDisposition, dwFlaghsAndAttributes, hTemplateFile);

}

HANDLE Client::Create() {
	return readReady = CreateEvent(NULL, 0, 0, NULL);
}

void Client::InitializeOverlappedStructure() {
	
	ZeroMemory(&OverlRd, sizeof(OverlRd));
	ResetEvent(readReady);
	OverlRd.hEvent = readReady;
}

void Client::Read(){

	fSuccess = ReadFile(hPipe, &msg, msg_sz, cbBytesRead, &OverlRd);
	
}

void Client::Wait() {

	WaitForSingleObject(hPipe, INFINITE);

}


