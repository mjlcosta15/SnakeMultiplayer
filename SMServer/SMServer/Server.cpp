#include "Server.h"

unsigned int __stdcall ThreadSharedMemoryReader(void * p)
{
	Server * server = (Server *) p;

	while (server->getSharedMemFlag()) {
		//Ler da memoria partilhada e imprimir no ecra
	}

	tcout << "ThreadSharedMemoryReader Closed" << endl;
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
	_beginthreadex(0, 0, ThreadSharedMemoryReader, this, 0, &smThreadID);
	hThreadSharedMemory = OpenThread(THREAD_ALL_ACCESS, FALSE, smThreadID);

	serverMainLoop();
}

void Server::serverMainLoop()
{	
	tstring msg;
	tcout << "Server Online." << "Write \"exit\" to shutdown the server" << endl;
	do {
		tcin >> msg;

		treatCommands(msg);

	} while (msg != TEXT("exit"));

	finishServer();
}

void Server::finishServer()
{
	_tprintf(TEXT("Server Exit, see ya!\n"));
	threadSharedMemFlag = false;
	WaitForSingleObject(hThreadSharedMemory, INFINITE);
	smHelper.finishSharedMemory();
	CloseHandle(hThreadSharedMemory);
}

void Server::treatCommands(tstring command)
{
}


bool Server::getSharedMemFlag() const {
	return threadSharedMemFlag;
}

