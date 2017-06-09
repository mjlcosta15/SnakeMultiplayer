#include "ServerPipeControl.h"



ServerPipeControl::ServerPipeControl()
{
	// Create Pipe
	this->lpName = TEXT("\\\\.\\pipe\\exemplo");	//tem de ser alterado
	this->dwOpenMode = PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED;	// está correto
	this->dwPipeMode = PIPE_TYPE_MESSAGE;
	this->nMaxInstances = PIPE_UNLIMITED_INSTANCES;
	this->nOutBufferSize = BUFSIZE;
	this->nDefaultTimeOut = 5000;
	this->lpSecurityAttributes = NULL;

	// Connect Pipe

	this->hNamedPipe = Create();

}


ServerPipeControl::~ServerPipeControl()
{
}

HANDLE ServerPipeControl::Create() {
	return CreateNamedPipe(lpName, 
		dwOpenMode,
		dwPipeMode,
		nMaxInstances,
		nOutBufferSize,
		nInBufferSize,
		nDefaultTimeOut,
		lpSecurityAttributes);
}

BOOL ServerPipeControl::Connect() {
	return ConnectNamedPipe(hNamedPipe, NULL);
}

BOOL ServerPipeControl::Disconnect() {
	return DisconnectNamedPipe(hNamedPipe);
}

int ServerPipeControl::Write(HANDLE hPipe, Message msg) {

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
		_tprintf(TEXT("\nNao chegou tudo"), GetLastError());

	return 1;

}

void ServerPipeControl::addClient(HANDLE cli) {

	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == NULL) {
			clients[i] = cli;
			return;
		}
	}

}

void ServerPipeControl::rmClient(HANDLE cli) {

	int i;
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (clients[i] == cli) {
			clients[i] = NULL; // O close HANDLE é feito na thread desse cliente
			return;
;		}
	}
}

int ServerPipeControl::Broadcast(Message msg) {

	int i, numwrites = 0;

	for (i = 0; i < MAX_PLAYERS; i++){
		if (clients[i] != 0)
			numwrites += Write(clients[i], msg);
	}

}


