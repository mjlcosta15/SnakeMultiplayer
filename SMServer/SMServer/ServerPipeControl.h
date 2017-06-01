#pragma once

//#include "windows.h" foste tu que meteste isto Mário?
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#include <io.h> // _setmode
#include <fcntl.h> // macro para o _setmode

#include "MainHeader.h" // as estruturas estão declaradas aqui

#define BUFSIZE 2048

#define QUEMSZ 60
#define MSGTXTSZ 60

// Vamos usar uma arquitetura cliente-servidor Overlapped I/O
// pois o pipe do cliente tem de conseguir ler e escrever em simultaneo

/*----Estruturas no MainHeader.h!----*/

class ServerPipeControl
{
private:
	LPCTSTR lpName;
	DWORD dwOpenMode;
	DWORD dwPipeMode;
	DWORD nMaxInstances;
	DWORD nOutBufferSize;
	DWORD nInBufferSize;
	DWORD nDefaultTimeOut;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes;


public:
	ServerPipeControl();
	~ServerPipeControl();

	HANDLE WINAPI CreateNamedPipe(
		LPCTSTR lpName,
		DWORD dwOpenMode,
		DWORD dwPipeMode,
		DWORD nMaxInstances,
		DWORD nOutBufferSize,
		DWORD nInBufferSize,
		DWORD nDefaultTimeOut,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes
	);
	BOOL WINAPI ConnectNamedPipe(HANDLE hNamedPipe,	LPOVERLAPPED lpOverlapped);
	BOOL WINAPI DisconnectNamedPipe(HANDLE hNamedPipe);

	void ReadFile();
	void WriteFile();

};

