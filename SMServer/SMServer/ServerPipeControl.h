#pragma once

#include "windows.h"

class ServerPipeControl
{
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

	BOOL WINAPI ConnectNamedPipe(
		HANDLE hNamedPipe,
		LPOVERLAPPED lpOverlapped
	);

	BOOL WINAPI DisconnectNamedPipe(
		HANDLE hNamedPipe
	);

	void ReadFile();
	void WriteFile();
};

