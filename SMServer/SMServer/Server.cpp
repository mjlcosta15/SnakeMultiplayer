#include "Server.h"

unsigned int __stdcall ThreadSharedMemoryReader(void * p)
{
	Server * server = (Server *)p;

	while (server->getSharedMemFlag()) {
		//Ler da memoria partilhada e imprimir no ecra
	}

	tcout << "ThreadSharedMemoryReader Closed" << endl;
	return 0;
}


Server::Server()
{
	game = Game();
	game.setInitalPhase();
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

void Server::startGame()
{
	game.setInProgressPhase();
	game.initMap();
}


bool Server::commandParser(vector<string> command)
{
	if (!command.size())
		return false;

	//commandos
	if (command[0] == "START") {
		if (!game.getGamePhase() == INITIAL_PHASE) {
			return true;
		}
		return false;
	}
	//DIFICULTY <number(1,99)>
	/*else if (command[0] == "LEVEL") {
		if (!missionStarted) {
			if (command.size() == 2) {
				if (stoi(command[1]) >= 1 && stoi(command[1]) <= 99)
					return true;
				return false;
			}
			return false;
		}
		return false;
	}
	//SETROOM <NAME> <ROOMNAME>
	else if (command[0] == "SETROOM") {
		if (!missionStarted) {
			if (command.size() == 3) {
				if (stoi(command[2]) >= 1 && stoi(command[2]) <= 12) {
					if (command[1] == "REPAIRER")
						return true;
					if (command[1] == "BEDS")
						return true;
					if (command[1] == "INFIRMARY")
						return true;
					if (command[1] == "PROP")
						return true;
					if (command[1] == "ROBOT")
						return true;
					if (command[1] == "SECSYSTEM")
						return true;
					if (command[1] == "CAPTAIN")
						return true;
					if (command[1] == "LASER")
						return true;
					if (command[1] == "GUN")
						return true;
				}
				return false;
			}
			return false;
		}
		return false;
	}
	//MOVE <ID> <NEXTROOM>
	else if (command[0] == "MOVE") {
		if (command.size() == 3) {
			if (missionStarted)
				if (stoi(command[2]) > 0 && stoi(command[2]) < 13)
					return true;
			return false;
		}
		return false;
	}
	//HELP
	else if (command[0] == "HELP") {
		return true;
	}
	//NEXT
	else if (command[0] == "NEXT") {
		if (missionStarted)
			return true;
	}
	else if (command[0] == "MAKESHIP")
		return true;
	else {
		//WRONG COMMAND
		return false;
	}*/
	return false;
}


void Server::treatCommand(vector<string> command)
{
	if (command[0] == "START") {

	}
	else if (command[0] == "ADDPLAYER") {
		//game.addPlayer(this));
	}
	else if (command[0] == "SETNAME") {

	}
	else if (command[0] == "SETDIRECTION") {

	}

}


string Server::commandToUpperCase(string command)
{
	for (unsigned int i = 0; i < command.size(); i++) {
		if (command[i] >= 'a' && command[i] <= 'z') {
			command[i] -= 32;
		}
	}
	return command;
}



bool Server::getSharedMemFlag() const {
	return threadSharedMemFlag;
}

