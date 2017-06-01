#include "ServerPipeControl.h"



ServerPipeControl::ServerPipeControl()
{
	// pq rai aparece este 'W' no fim do nome da func?
	this->CreateNamedPipeW(TEXT("\\\\.\\pipe\\exemplo"),	//tem de ser alterado
						PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,	// está correto
						PIPE_TYPE_MESSAGE,
						PIPE_UNLIMITED_INSTANCES,
						BUFSIZE,
						BUFSIZE,
						5000,
						NULL);



}


ServerPipeControl::~ServerPipeControl()
{
}

HANDLE ServerPipeControl::CreateNamedPipe(LPCTSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	this->lpName = lpName;
	this->dwOpenMode = dwOpenMode; 
	this->dwPipeMode;
	nMaxInstances;
	nOutBufferSize;
	nInBufferSize;
	nDefaultTimeOut;
	lpSecurityAttributes;

	return HANDLE();
}

BOOL ServerPipeControl::ConnectNamedPipe(HANDLE hNamedPipe, LPOVERLAPPED lpOverlapped)
{
	return 0;
}

BOOL ServerPipeControl::DisconnectNamedPipe(HANDLE hNamedPipe)
{
	return 0;
}


