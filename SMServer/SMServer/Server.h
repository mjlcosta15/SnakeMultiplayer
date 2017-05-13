#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "SharedMemoryHelper.h"
#include "Map.h"

class Server
{
	SharedMemoryHelper smHelper;
	HANDLE hThreadSharedMemory;
	bool threadSharedMemFlag;
	unsigned int smThreadID;
public:
	Server();
	~Server();
	void startServer();
	void finishServer();
	bool getSharedMemFlag() const;
};

#endif // !SERVER_H