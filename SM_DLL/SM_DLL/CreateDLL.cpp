#include "CreateDLL.h"

HANDLE hMutex = NULL;
HANDLE hEvent = NULL;
HANDLE hMapFile = NULL;

static const TCHAR szName[] = TEXT("SnakeMultiplayerSharedMem");


SNAKE_MULTIPLAYER_API Message * ReadFromSharedMemoryBuffer(void)
{
	Message * msg;
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return nullptr;
	}

	//cout << "estou à espera de eventos" << endl;
	WaitForSingleObject(hEvent, INFINITE);

	msg = (Message *)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(Message));

	if (msg == NULL)
	{
		//_tprintf(TEXT("Could not map view of file (%d).\n"),
		//	GetLastError());

		CloseHandle(hMapFile);
		return nullptr;
	}


	UnmapViewOfFile(msg);

	CloseHandle(hMapFile);

	return msg;
}

SNAKE_MULTIPLAYER_API bool WriteToSharedMemoryBuffer(Message msg)
{
	Message * ptrmsg;
	if (hEvent == NULL) {
		hEvent = CreateEvent(
			NULL,               // default security attributes
			TRUE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			TEXT("DLL_SHARED_MEMORY_EVENT")  // object name
		);
		if (hEvent == NULL)
		{
			printf("CreateEvent failed (%d)\n", GetLastError());
			return false;
		}
	}

	if (hMutex == NULL) {
		hMutex = CreateMutex(
			NULL,
			FALSE,
			NULL
		);

		if (hMutex == NULL) {
			printf("CreateMutex failed (%d)\n", GetLastError());
			return false;
		}
	}


	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return false;
	}

	ptrmsg = (Message *)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(Message));

	if (ptrmsg == NULL)
	{
		//_tprintf(TEXT("Could not map view of file (%d).\n"),
		//	GetLastError());

		CloseHandle(hMapFile);
		return false;
	}


	WaitForSingleObject(hMutex, INFINITE);

	CopyMemory(ptrmsg, &msg, sizeof(Message));

	ReleaseMutex(hMutex);

	SetEvent(hEvent);

	UnmapViewOfFile(ptrmsg);

	CloseHandle(hMapFile);
	return true;
}


