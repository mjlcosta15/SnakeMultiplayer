#include "ClientPipeControl.h"



ClientPipeControl::ClientPipeControl()
{
	Create(); // liga-se ao pipe
	InitializeOverlappedStructure();
	Wait();
}


ClientPipeControl::~ClientPipeControl()
{
}

void ClientPipeControl::OpenPipe() {

	hPipe = CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreaationDisposition, dwFlaghsAndAttributes, hTemplateFile);

}

HANDLE ClientPipeControl::Create() {
	return readReady = CreateEvent(NULL, 0, 0, NULL);
}

void ClientPipeControl::InitializeOverlappedStructure() {
	
	ZeroMemory(&OverlRd, sizeof(OverlRd));
	ResetEvent(readReady);
	OverlRd.hEvent = readReady;
}

void ClientPipeControl::Read(){

	fSuccess = ReadFile(hPipe, &msg, msg_sz, cbBytesRead, &OverlRd);
	
}

void ClientPipeControl::Wait() {

	WaitForSingleObject(hPipe, INFINITE);

}


