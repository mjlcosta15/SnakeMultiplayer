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
	this->lpOverlapped;


}


ServerPipeControl::~ServerPipeControl()
{
}

HANDLE ServerPipeControl::Create() {
	return CreateNamedPipe(lpName, dwOpenMode, dwPipeMode, nMaxInstances, nOutBufferSize, nInBufferSize,  nDefaultTimeOut, lpSecurityAttributes);
}

BOOL ServerPipeControl::Connect() {
	return ConnectNamedPipe(hNamedPipe, lpOverlapped);
}

BOOL ServerPipeControl::Disconnect() {
	return DisconnectNamedPipe(hNamedPipe);
}


