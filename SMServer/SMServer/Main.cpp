#include "Server.h"

using namespace std;

// Isto é um exemplo dos slides, nao deve ser o mais correto

/*
DWORD WINAPI Thread(LPVOID lpParam) {

	// local variables
	// function code

	return value;
}

HANDLE Thread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,	// Security Description
	DWORD dwStackSize,							// Minimum Stack Size
	LPTHREAD_START_ROUTINE lp_star_address,		// Thread Function
	LPVOID lpParamter,							// Function Parameter
	DWORD dwCreationFlags,						// Creation Options
	LPWORD lpThreadId							// Ptr Thread ID

);*/


int _tmain()
{
	BOOL fConnected = FALSE;
	DWORD dwThreadId = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
	LPTSTR lpszPipename = TEXT("\\\\\.\\pipe\\pipeeexemplo");

	

	Server server;
	server.startServer();

	
}



