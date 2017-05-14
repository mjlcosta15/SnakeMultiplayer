#include "SharedMemoryHelper.h"
static const TCHAR szName[] = TEXT("SnakeMultiplayerSharedMem");


tstring Msg::getBuffer() const
{
	return szBuffer;
}


SharedMemoryHelper::SharedMemoryHelper()
{
}


SharedMemoryHelper::~SharedMemoryHelper()
{
}

bool SharedMemoryHelper::initSharedMemory()
{

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		SM_BUFFER_SIZE,
		szName);

	if (hMapFile == NULL) {
		_tprintf(TEXT("[ERROR] Creating Shared Memory - %d\n"), GetLastError());
		return false;
	}
	else {
		_tprintf(TEXT("Shared Memory created with success\n"));
	}

	szBuffer = (Msg *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, SM_BUFFER_SIZE);

	if (szBuffer == NULL) {
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

Msg* SharedMemoryHelper::getBuffer()
{
	return szBuffer;
}
