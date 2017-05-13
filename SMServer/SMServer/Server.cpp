#include "Server.h"

unsigned int __stdcall ThreadSharedMemoryReader(void * p)
{
	while (Server::getSharedMemFlag()) {
		//Ler da memoria partilhada e imprimir no ecra
	}
	return 0;
}


Server::Server()
{
}


Server::~Server()
{
}

void Server::startServer()
{
	if (!smHelper.initSharedMemory()) {
		finishServer();
	}
	//lançar a thread the leitura
	threadSharedMemFlag = true;
	_beginthreadex(0, 0, ThreadSharedMemoryReader, smHelper.getBuffer(), 0, &smThreadID);
	hThreadSharedMemory = OpenThread(THREAD_ALL_ACCESS, FALSE, smThreadID);

	threadSharedMemFlag = false;
	WaitForSingleObject(hThreadSharedMemory, INFINITE);
	smHelper.finishSharedMemory();
	CloseHandle(hThreadSharedMemory);
}

void Server::finishServer()
{
	_tprintf(TEXT("Server Exit, see ya!\n"));
}


bool Server::getSharedMemFlag() const {
	return threadSharedMemFlag;
}

