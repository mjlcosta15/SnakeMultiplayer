#pragma once

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <fcntl.h>

#include "MainHeader.h"

#define msg_sz sizeof(Message)


class Client
{

public:
	Client();
	~Client();

	void OpenPipe();

	// 1 - Cria/Liga
	HANDLE Create();

	// 2 - Inicializa
	void InitializeOverlappedStructure();

	void Read();

	void Wait();

	int start();



};

