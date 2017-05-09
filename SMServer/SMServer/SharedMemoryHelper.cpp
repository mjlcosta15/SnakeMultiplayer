#include "SharedMemoryHelper.h"



SharedMemoryHelper::SharedMemoryHelper()
{
	// ESTE CODIGO É DO MSDN

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
	}
	else {
		pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
			FILE_MAP_ALL_ACCESS, // read/write permission
			0,
			0,
			BUF_SIZE);

		if (pBuf == NULL)
		{
			_tprintf(TEXT("Could not map view of file (%d).\n"),GetLastError());
			CloseHandle(hMapFile);
		}
		else {
			CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
			_getch();
			UnmapViewOfFile(pBuf);
			CloseHandle(hMapFile);
		}
	}
}


SharedMemoryHelper::~SharedMemoryHelper()
{
}
