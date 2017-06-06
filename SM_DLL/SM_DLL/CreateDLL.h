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

class CreateDLL
{
	Message * msg;
	HANDLE hMapFile;
public:

	CreateDLL();
	~CreateDLL();

	SNAKE_MULTIPLAYER_API Message * ReadFromSharedMemoryBuffer();

	SNAKE_MULTIPLAYER_API bool WriteToSharedMemoryBuffer(Message msg);

};

SNAKE_MULTIPLAYER_API HANDLE hMutex;
SNAKE_MULTIPLAYER_API HANDLE hEvent;


#endif //__CREATEDLL_H__