#pragma once

#include <windows.h>

#include "MainHeader.h"

#define msg_sz sizeof(Message)


class ClientPipeControl
{
private:
	LPCTSTR lpFileName;
	DWORD dwDesiredAccess;
	DWORD dwShareMode;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes;
	DWORD dwCreaationDisposition;
	DWORD dwFlaghsAndAttributes;
	HANDLE hTemplateFile;

	LPCSTR lpNamedPipeName;
	DWORD nTimeOut;

	HANDLE readReady;
	OVERLAPPED OverlRd;

	HANDLE hPipe;

	BOOL fSuccess;

	Message msg;
	LPDWORD cbBytesRead;




public:
	ClientPipeControl();
	~ClientPipeControl();

	void OpenPipe();

	// 1 - Cria/Liga
	HANDLE Create();

	// 2 - Inicializa
	void InitializeOverlappedStructure();

	void Read();

	void Wait();



};

