#include "CreateDLL.h"

static const TCHAR szName[] = TEXT("SnakeMultiplayerSharedMem");

CreateDLL::CreateDLL()
{
}

CreateDLL::~CreateDLL()
{
}

SNAKE_MULTIPLAYER_API Message * CreateDLL::ReadFromSharedMemoryBuffer()
{


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

	msg = (Message *)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(Message));

	if (msg == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);
		return nullptr;
	}


	UnmapViewOfFile(msg);

	CloseHandle(hMapFile);

	return msg;
}

SNAKE_MULTIPLAYER_API bool CreateDLL::WriteToSharedMemoryBuffer(Message msg)
{

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
	
	this->msg = (Message *)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		sizeof(Message));

	if (this->msg == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);
		return false;
	}

	
		
	CopyMemory(this->msg, &msg, sizeof(Message));


	UnmapViewOfFile(this->msg);

	CloseHandle(hMapFile);
	return true;
}
