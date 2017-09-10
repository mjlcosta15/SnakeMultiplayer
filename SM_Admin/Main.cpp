#include "MainHeader.h"
using namespace std;

#define msg_sz sizeof(Message)

int _tmain()
{
	HANDLE hPipe;
	DWORD dwMode;
	BOOL fSuccess = false;
	DWORD cbWritten;
	LPTSTR pipeNameAdmin = TEXT("\\\\.\\pipe\\pipeAdmin");
	Message msg;

	hPipe = CreateFile(
		pipeNameAdmin, // Nome do pipe remoto
		GENERIC_READ | // acesso read e write
		GENERIC_WRITE,
		0 | FILE_SHARE_READ | FILE_SHARE_WRITE, // sem -> com partilha
		NULL,
		OPEN_EXISTING,
		0 | FILE_FLAG_OVERLAPPED,
		NULL);

	if (hPipe != INVALID_HANDLE_VALUE) {
		_tprintf(TEXT("Erro = %d\nErro ao criar o pipe!\n"), GetLastError());
		return -1;
	}


	if (GetLastError() != ERROR_PIPE_BUSY) {
		DWORD i = GetLastError();
		_tprintf(TEXT("\nCreate file deu erro e nao foi BUSY. Erro = %d\n"), GetLastError());
		return -1;


	}

	if (!WaitNamedPipe(pipeNameAdmin, 30000)) {
		_tprintf(TEXT("Game not started yet! Try again later.\n"));
		return -1;

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


	string cmd;
	HANDLE WriteReady; // Handle para o evento da leitura (cada thread tem um)
	OVERLAPPED OverlWr = { 0 };
	do {

	

	//Envia a primeira mensagem
	

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
	
	} while (cmd != "SAIR");

	CloseHandle(WriteReady);
	CloseHandle(hPipe);
}
