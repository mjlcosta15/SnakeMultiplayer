#pragma once

#include <windows.h>

class ClientPipeControl
{
public:
	ClientPipeControl();
	~ClientPipeControl();

	HANDLE WINAPI CallNamedPipe(
		LPCTSTR lpFileName,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreaationDisposition,
		DWORD dwFlaghsAndAttributes,
		HANDLE hTemplateFile
	);

	BOOL WINAPI WaitNamedPipe(
		LPCSTR lpNamedPipeName,
		DWORD nTimeOut
	);

	void ReadFile();
	void WriteFile();
};

