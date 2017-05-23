#ifndef __SHARED_MEMORY_HELPER_H__
#define __SHARED_MEMORY_HELPER_H__

#include "MainHeader.h"

#define SM_BUFFER_SIZE 2048

class Msg {
	TCHAR szBuffer[SM_BUFFER_SIZE];
public:
	 tstring getBuffer() const;
};

class SharedMemoryHelper
{
	HANDLE hMapFile;
	Msg * szBuffer;
public:
	SharedMemoryHelper();
	~SharedMemoryHelper();

	bool initSharedMemory();

	bool finishSharedMemory();
	Msg* getBuffer();
};

#endif //