// Como criar uma DLL

#pragma once
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUF_SIZE 256
TCHAR szName[] = TEXT("Global\\SnakeMultiplayerSharedMemory");

#ifdef AULADLL_EXPORTS
#define DLL_IMP_API __declspec(dllexport)
#else
#define DLL_IMP_API __declspec(dllimport)
#endif
extern "C" {
	extern DLL_IMP_API HANDLE hMapFile;;

	DLL_IMP_API int readFromSharedMemory();

	DLL_IMP_API int writeOnSharedMemory(void);
}