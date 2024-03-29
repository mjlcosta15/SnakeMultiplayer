#include "Client.h"
#define msg_sz sizeof(Message)
LPCTSTR lpFileName;
DWORD dwDesiredAccess;
DWORD dwShareMode;
LPSECURITY_ATTRIBUTES lpSecurityAttributes;
DWORD dwCreationDisposition;
DWORD dwFlaghsAndAttributes;
HANDLE hTemplateFile;

LPCSTR lpNamedPipeName;
DWORD nTimeOut;

HANDLE readReady;
OVERLAPPED OverlRd;

HANDLE hPipe;


//Message msg;
LPDWORD cbBytesRead;
LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\pipeexemplo");






int DeveContinuar = 1;
int ReaderAlive = 0;

DWORD WINAPI ThreadClientReader(LPVOID lpvParam) {
	Message FromServer;

	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a informa�ao enviada � o handle

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	if (hPipe == NULL) {
		_tprintf(TEXT("\nThreadReader - o handle recibo no param da thread � nulo\n"));
		return -1;
	}

	ReadReady = CreateEvent(
		NULL, // default security
		TRUE,	// reset manual, por requisito do overlapped IO
		FALSE,	// estado inicial = not signaled
		NULL);	// nao precisa de nome. Uso interno ao processo
	
	if (ReadReady == NULL) {
		_tprintf(TEXT("\nCliente: nao foi poss�vel criar o Evento Read. Mais vale parar j�"));
		return 1;
	}

	// Ciclo de di�logo com o cliente

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

DWORD WINAPI ThreadClientWrite(LPVOID lpvParam) {
	Message FromServer;

	DWORD cbBytesRead = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = (HANDLE)lpvParam; // a informa�ao enviada � o handle

	HANDLE ReadReady;
	OVERLAPPED OverlRd = { 0 };

	if (hPipe == NULL) {
		_tprintf(TEXT("\nThreadReader - o handle recibo no param da thread � nulo\n"));
		return -1;
	}

	ReadReady = CreateEvent(
		NULL, // default security
		TRUE,	// reset manual, por requisito do overlapped IO
		FALSE,	// estado inicial = not signaled
		NULL);	// nao precisa de nome. Uso interno ao processo

	if (ReadReady == NULL) {
		_tprintf(TEXT("\nCliente: nao foi poss�vel criar o Evento Read. Mais vale parar j�"));
		return 1;
	}

	// Ciclo de di�logo com o cliente

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
	
	//Message msg;
	Message MsgToSend;

	_setmode(_fileno(stdout), _O_WTEXT); // proteger com #ifdef UNICODE ... #endif

	_tprintf(TEXT("Escreve nome >"));
	//readTChars(MsgToSend.quem, QUEMSZ);

	//Create(); // liga-se ao pipe
	//InitializeOverlappedStructure();
	//Wait();

	//***********Pipe Remoto************

	HANDLE hUserToken = NULL;
	BOOL log;
	int ret;

	TCHAR username[20],	// username da m�quina destino
		pass[20],			// password desse utilizador (na m�quina destino)
		dominio[20];		// pode ser o IP da m�quina


	_tcscpy(dominio, TEXT("192.168.1.82"));
	_tcscpy(username, TEXT("diogosantos"));
	_tcscpy(pass, TEXT("q1w2e3r4"));

	//_tcscpy(lpszPipename, TEXT("\\\\"));
	//_tcscat(lpszPipename, dominio);
	lpszPipename = TEXT("\\\\192.168.1.82\\pipe\\pipeexemplo");

	log = LogonUser(username, dominio, pass,
		LOGON32_LOGON_NEW_CREDENTIALS,	// tipo de logon
		LOGON32_PROVIDER_DEFAULT,		// logon provider
		&hUserToken);

	log = ImpersonateLoggedOnUser(hUserToken);

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
			DWORD i = GetLastError();
			_tprintf(TEXT("\nCreate file deu erro e nao foi BUSY. Erro = %d\n"), GetLastError());
			return -1;
		}

		// Se chegou aqui � porque todas as inst�ancias
		// do pipe est�o ocupadas. Rem�dio: aguardar que uma
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
		NULL,		// Nao � para mudar max. bytes
		NULL);		// Nao e para mudar max. timeout

	if (!fSuccess) {
		_tprintf(TEXT("SetNamedPipeHandleState falhou. Erro = %d\n"), GetLastError());
		return -1;
	}

	hThread = CreateThread(
		NULL,
		0,
		ThreadClientReader,
		(LPVOID)hPipe,
		0,
		&dwThreadId);

	if (hThread == NULL) {
		_tprintf(TEXT("\nErro na cria��o da thread. Erro = %d"), GetLastError());
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
		_tprintf(TEXT("\nCliente: n�o foi poss�vel criar o Evento. Mais vale parar j�"));
		return 1;
	}

	_tprintf(TEXT("\nliga��o estabelecida. \"exit\" para sair"));
	MsgToSend.pid = 69;
	MsgToSend.scores[1] = 2;
	MsgToSend.scores[2] = 25;
	strcpy(MsgToSend.msg, TEXT("start\n"));
	MsgToSend.msg[_tcslen(MsgToSend.msg) - 1] = TEXT('\0');
	
	while (1) {

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
		_tprintf(TEXT("\nCliente vai terminar liga��o e sair"));

	}

	_tprintf(TEXT("\nCleinte vai terminar liga��o e sair"));
	CloseHandle(WriteReady);
	CloseHandle(hPipe);
	return 0;

	

}



