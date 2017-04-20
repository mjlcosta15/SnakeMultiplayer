#include <iostream>
#include <windows.h>

#include "Map.h"

using namespace std;

// Isto é um exemplo dos slides, nao deve ser o mais correto

/*
DWORD WINAPI Thread(LPVOID lpParam) {

	// local variables
	// function code

	return value;
}

HANDLE Thread(
	LPSECURITY_ATTRIBUTES lpThreadAttributes,	// Security Description
	DWORD dwStackSize,							// Minimum Stack Size
	LPTHREAD_START_ROUTINE lp_star_address,		// Thread Function
	LPVOID lpParamter,							// Function Parameter
	DWORD dwCreationFlags,						// Creation Options
	LPWORD lpThreadId							// Ptr Thread ID

);*/




int main()
{
	cout << "Hello World!" << endl;

	Map map("\lvl1.txt");
}