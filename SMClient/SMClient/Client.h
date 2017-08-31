#pragma once

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <fcntl.h>

#include "MainHeader.h"




class Client
{

public:
	Client();
	~Client();

	int start();

};

