#include "SharedMemoryHelper.h"
static const TCHAR szName[] = TEXT("SnakeMultiplayerSharedMem");


SharedMemoryHelper::SharedMemoryHelper()
{
}


SharedMemoryHelper::~SharedMemoryHelper()
{
}

bool SharedMemoryHelper::initSharedMemory()
{
	volatile LPCSTR pBuf;
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(Message),
		szName);

	if (hMapFile == NULL) {
		_tprintf(TEXT("[ERROR] Creating Shared Memory - %d\n"), GetLastError());
		return false;
	}
	else {
		_tprintf(TEXT("Shared Memory created with success\n"));
	}

	pBuf = (LPCSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Message));

	if (pBuf == NULL) {
		_tprintf(TEXT("[ERROR] Getting view of Shared Memory - %d\n"), GetLastError());
		CloseHandle(hMapFile);
		return false;
	}

	return true;
}

bool SharedMemoryHelper::finishSharedMemory()
{
	_tprintf(TEXT("Unmaping Shared memory\n"));
	UnmapViewOfFile(hMapFile);
	return UnmapViewOfFile(hMapFile);
}

