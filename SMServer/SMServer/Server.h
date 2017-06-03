#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "SharedMemoryHelper.h"
#include "Game.h"

class Server
{
	Game game;
	SharedMemoryHelper smHelper;
	HANDLE hThreadSharedMemory;
	bool threadSharedMemFlag;
	unsigned int smThreadID;
public:
	Server();
	~Server();
	void startServer();
	void serverMainLoop();
	void finishServer();
	
	void startGame();


	//Function that validates the command inputed
	bool commandParser(vector<string> command);
	//Function that process the requested command
	void treatCommand(vector<string> command);
	//Function to convert all string characters into upper case version
	string commandToUpperCase(string command);

	bool getSharedMemFlag() const;
};

#endif // !SERVER_H