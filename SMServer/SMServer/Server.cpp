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

	//START
	if (command[0] == "START") {
		if (!game.getGamePhase() == INITIAL_PHASE) {
			return true;
		}
		return false;
	}
	//CREATEGAME <Map Width [1,80]> <Map Height [1,80]> <Nr Players [1,10]> <Snake Size [1,10]> <Nr Objects [1,10]> <Nr Snakes AI [1,10]> <Player Username>
	else if (command[0] == "CREATEGAME") {
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 8) {
				if (stoi(command[1]) >= 1 && stoi(command[1]) <= MAX_TAM_MAP) {
					if (stoi(command[2]) >= 1 && stoi(command[2]) <= MAX_TAM_MAP) {
						if (stoi(command[3]) >= 1 && stoi(command[3]) <= MAX_PLAYERS) {
							if (stoi(command[4]) >= 1 && stoi(command[4]) <= MAX_PLAYERS) {
								if (stoi(command[5]) >= 1 && stoi(command[5]) <= MAX_PLAYERS) {
									if (stoi(command[6]) >= 1 && stoi(command[6]) <= MAX_PLAYERS) {
										if (command[7] != "") {
											return true;
										}
										else {
											cout << "COMMAND -" << command[0] << "- Error in Username" << endl;

										}
									}
									else {
										cout << "COMMAND -" << command[0] << "- Error in Nr Snakes AI" << endl;

									}
								}
								else {
									cout << "COMMAND -" << command[0] << "- Error in Nr Objects" << endl;
								}
							}
							else {
								cout << "COMMAND -" << command[0] << "- Error in Snake Size" << endl;
							}
						}
						else {
							cout << "COMMAND -" << command[0] << "- Error in Nr Players" << endl;
						}
					}
					else {
						cout << "COMMAND -" << command[0] << "- Error in Map Height" << endl;
					}
				}
				else {
					cout << "COMMAND -" << command[0] << "- Error in Map Width" << endl;
				}
				return false;
			}
			return false;
		}
		return false;
	}//JOIN <username>
	else if (command[0] == "JOIN") {
		if (game.getGamePhase() == INITIAL_PHASE) {
			if (command.size() == 2)
				if (command[1] != "")
					return true;
				else
					cout << "COMMAND -" << command[0] << "- Error in Username" << endl;
			return false;
		}
		return false;
	}
	//SETDIRECTION <Direction(1,2,3,4)>
	else if (command[0] == "SETDIRECTION") {
		if (command.size() == 2) {
			if (game.getGamePhase() == IN_PROGRESS_PHASE)
				if (stoi(command[1]) > 0 && stoi(command[1]) < GOING_RIGHT)
					return true;
				else
					cout << "COMMAND -" << command[0] << "- Error in Direction" << endl;
			return false;
		}
		return false;
	}//DISCONNECT <PID>
	else if (command[0] == "DISCONNECT") {
		if (command.size() == 2) {
			char* p;
			strtol(command[1].c_str(), &p, 10);
			if (*p == 0)
				return true;
			else
				cout << "COMMAND -" << command[0] << "- Error in PID" << endl;
			return false;
		}
		return false;
	}
	else {
		//WRONG COMMAND
		return false;
	}
	return false;
}


void Server::treatCommand(vector<string> command, Message msg)
{
	if (command[0] == "START") {
		game.setInProgressPhase();
	}
	else if (command[0] == "CREATEGAME") {
		game.setMapWidth(stoi(command[1]));
		game.setMapHeight(stoi(command[2]));
		game.setNumPlayers(stoi(command[3]));
		game.setSnakeSize(stoi(command[4]));
		game.setNumberOfObjects(stoi(command[5]));
		game.setNumSnakesAI(stoi(command[6]));
		game.addPlayer(new Player(msg.pid, command[7], &game));
	}
	else if (command[0] == "JOIN") {
		game.addPlayer(new Player(msg.pid, command[1], &game));
	}
	else if (command[0] == "SETDIRECTION") {
		game.setDirectionToPlayer(msg.pid, stoi(command[1]));
	}
	else if (command[0] == "DISCONNECT") {
		game.removePlayer(msg.pid);
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

WINAPI DWORD threadShared