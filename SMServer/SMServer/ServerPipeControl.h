#pragma once

//#include "windows.h" foste tu que meteste isto Mário?
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#include <io.h> // _setmode
#include <fcntl.h> // macro para o _setmode

#include "MainHeader.h" // as estruturas estão declaradas aqui

#define BUFSIZE 2048

#define QUEMSZ 60
#define MSGTXTSZ 60

#define msg_sz sizeof(Message)

// Vamos usar uma arquitetura cliente-servidor Overlapped I/O
// pois o pipe do cliente tem de conseguir ler e escrever em simultaneo

/*----Estruturas no MainHeader.h!----*/

class ServerPipeControl
{
private:
	
	


public:
	ServerPipeControl();
	~ServerPipeControl();

	



};

