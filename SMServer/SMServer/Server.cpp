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
	/*game.setInitalPhase();
	game.setMapHeight(10);
	game.setMapWidth(10);
	game.setNumSnakesAI(3);
	game.setNumberOfObjects(3);
	game.setSnakeSize(3);
	game.addPlayer(1, "jorge");
	game.initMap();
	game.setInProgressPhase();

	game.updateMap();
	game.updateMap();
	game.updateMap();
	game.updateMap();
	game.updateMap();
	game.updateMap();
	game.updateMap();
	game.setFinishPhase();*/
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

	tcout << "Server Online." << endl;
	game.setInitalPhase();

	do {
		
		//Initial Phase
		if (game.getGamePhase() == INITIAL_PHASE)
			initialPhaseLoop();
		//Game Phase
		if (game.getGamePhase() == IN_PROGRESS_PHASE)
			GamePhaseLoop();

	} while (game.getGamePhase() == FINISH_PHASE);
	//Finish Phase
	finishServer();
}

void Server::initialPhaseLoop()
{
	tcout << "Initial Phase Loop started" << endl;
	
	do {

		game.setMapHeight(10);
		game.setMapWidth(10);
		game.setNumSnakesAI(3);
		game.setNumberOfObjects(3);
		game.setSnakeSize(3);
		game.addPlayer(1, "jorge");
		game.initMap();
		game.setInProgressPhase();

	} while (game.getGamePhase() == INITIAL_PHASE);
}

void Server::GamePhaseLoop()
{
	tcout << "Game Phase Loop started" << endl;
	do {
		game.updateMap();
		Sleep(33); //Fazer 30 atualizações por segundo (30 FPS oh yeah)
	} while (game.getGamePhase() == IN_PROGRESS_PHASE);
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
		game.setInProgressPhase();
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

