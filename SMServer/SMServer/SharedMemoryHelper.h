#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\SnakeMultiplayerSharedMemory");
TCHAR szMsg[] = TEXT("Message from first process.");

class SharedMemoryHelper
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
public:
	SharedMemoryHelper();
	~SharedMemoryHelper();
};

