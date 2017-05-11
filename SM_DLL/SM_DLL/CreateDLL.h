#ifndef __CREATEDLL_H__
#define __CREATEDLL_H__

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
using namespace std;

#ifdef SNAKE_MULTIPLAYER_EXPORTS  
#define SNAKE_MULTIPLAYER_API __declspec(dllexport)   
#else  
#define SNAKE_MULTIPLAYER_API __declspec(dllimport)   
#endif  

#define SM_BUFFER_SIZE 2048


class SharedMemoryHelper 
{
	HANDLE smMap;
public:
	// Returns a + b  
	SNAKE_MULTIPLAYER_API double CreateSharedMemBuffer();

	// Returns a * b  
	SNAKE_MULTIPLAYER_API double ReadFromSharedMemoryBuffer();

	// Returns a + (a * b)  
	SNAKE_MULTIPLAYER_API double WriteToSharedMemoryBuffer();
};


#endif //__CREATEDLL_H__