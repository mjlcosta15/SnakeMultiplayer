#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "SharedMemoryHelper.h"
#include "Map.h"
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

	void treatCommands(tstring command);
	bool getSharedMemFlag() const;
};

#endif // !SERVER_H