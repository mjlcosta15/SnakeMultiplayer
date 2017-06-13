#pragma once
#ifndef SERVER_H
#define SERVER_H
#define _WIN32_WINNT 0x0500 // necessário para o pipe remoto

#include <windows.h>
#include <sddl.h>

#include "SharedMemoryHelper.h"
#include "MainHeader.h"
#include "Game.h"

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
	bool commandParser(vector<string> command);
	//Function that process the requested command
	void treatCommand(vector<string> command, Message msg);
	//Function to convert all string characters into upper case version
	string commandToUpperCase(string command);

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

	static DWORD WINAPI InstanceThread(LPVOID lpvParam);
};

#endif // !SERVER_H