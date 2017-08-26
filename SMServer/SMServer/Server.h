#pragma once
#ifndef SERVER_H
#define SERVER_H
#define _WIN32_WINNT 0x0500 // necessário para o pipe remoto

#include "SharedMemoryHelper.h"
#include "MainHeader.h"
#include "Game.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>

#include <AclAPI.h>

#include <sddl.h>

#define BUFSIZE 2048

#define QUEMSZ 60
#define MSGTXTSZ 60

#define msg_sz sizeof(Message)

class Server
{
	
public:
	Server();
	~Server();

	void startServer();
	void serverMainLoop();

	void initialPhaseLoop();
	void GamePhaseLoop();

	void finishServer();
	
	void startGame();


	//Function that validates the command inputed
	static bool commandParser(vector<string> command);
	//Function that process the requested command
	static void treatCommand(vector<string> command, Message msg);
	//Function to convert all string characters into upper case version
	static string commandToUpperCase(string command);

	static vector<string> getCommand(char* buffer);

	bool getSharedMemFlag() const;

	int waitConnection();

	/*-------Funcoes principais-------*/
	// 1 - Criação do Pipe
	HANDLE Create();

	// 2 - Aguarda que um cliente se ligue
	BOOL Connect();
	BOOL Disconnect();

	// 3 - Envia mensagem a cliente
	static int Write(HANDLE hPipe, Message msg);

	// Regista novo cliente
	static void addClient(HANDLE client);

	// Remove registo
	static void rmClient(HANDLE client);

	// Manda alterações a todos os clientes
	static int Broadcast(Message msg);

	/*-------Funcoes auxiliares-------*/
	void setfConnected(BOOL flag);
	BOOL getfConncetd()const;

	HANDLE getClient(int pos)const;

	HANDLE getServerPipe();

	void initializeClients();

	//int waitConnection();

	void setHNamedPipe(HANDLE namedPipe);

	HANDLE getHNamedPipe();

	static DWORD WINAPI ThreadProcClient(LPVOID lpvParam);
};

#endif // !SERVER_H