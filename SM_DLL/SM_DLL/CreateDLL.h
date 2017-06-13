#pragma once
#ifndef __CREATEDLL_H__
#define __CREATEDLL_H__

#define _CRT_SECURE_NO_WARNINGS

#ifdef SM_DLL_EXPORTS
#define SNAKE_MULTIPLAYER_API __declspec(dllexport)
#else
#define SNAKE_MULTIPLAYER_API __declspec(dllimport)
#endif

#include "MainHeader.h"


extern "C" {

	SNAKE_MULTIPLAYER_API  Message * ReadFromSharedMemoryBuffer(void);

	SNAKE_MULTIPLAYER_API bool WriteToSharedMemoryBuffer(Message msg);

	extern SNAKE_MULTIPLAYER_API HANDLE hMapFile;
	extern SNAKE_MULTIPLAYER_API HANDLE hMutex;
	extern SNAKE_MULTIPLAYER_API HANDLE hEvent;

}
#endif //__CREATEDLL_H__