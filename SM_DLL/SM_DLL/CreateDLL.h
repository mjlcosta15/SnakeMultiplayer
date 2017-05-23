#ifndef __CREATEDLL_H__
#define __CREATEDLL_H__

#define _CRT_SECURE_NO_WARNINGS

#ifdef SNAKE_MULTIPLAYER_EXPORTS
#define SNAKE_MULTIPLAYER_API __declspec(dllexport) 
#else
#define SNAKE_MULTIPLAYER_API __declspec(dllimport) 
#endif

#include <windows.h>
#include <vector>
#include <tchar.h>
#include <sstream>

using namespace std;

#ifdef UNICODE 
#define tstring wstring
#define tstringstream wstringstream
#else
#define tstring string
#define tstringstream stringstream
#endif

#define SM_BUFFER_SIZE 2048

class Msg {
	TCHAR szBuffer[SM_BUFFER_SIZE];
public:
	SNAKE_MULTIPLAYER_API tstring getBuffer() const;
};

class CreateDLL
{
	
	Msg * szBuffer;
	HANDLE hMapFile;
public:

	CreateDLL();
	~CreateDLL();

	SNAKE_MULTIPLAYER_API bool ReadFromSharedMemoryBuffer();

	SNAKE_MULTIPLAYER_API bool WriteToSharedMemoryBuffer();
};


#endif //__CREATEDLL_H__