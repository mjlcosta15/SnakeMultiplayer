#ifndef __SHARED_MEMORY_HELPER_H__
#define __SHARED_MEMORY_HELPER_H__

#include "MainHeader.h"

class SharedMemoryHelper
{
	HANDLE hMapFile;
	Message message;
public:
	SharedMemoryHelper();
	~SharedMemoryHelper();

	bool initSharedMemory();

	bool finishSharedMemory();
	Message getBuffer();
};

#endif // SHARED_MEMORY_HELPER_H