#pragma once
#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

#define _CRT_SECURE_NO_WARNINGS

#define MAX_TAM_MAP 80
#define MAX_PLAYERS 10
#define BUFFER_SIZE 2048

#define FAIL			0

// Command Parser returns
#define START			1
#define CREATEGAME		2
#define JOIN			3
#define SETDIRECTION	4
#define DISCONNECT		5

#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <sstream>
#include <stdlib.h>
#include <time.h> 
#include <process.h>
#include <io.h>
#include <iostream>

using namespace std;

#ifdef UNICODE 
#define tcout wcout
#define tcin wcin
#define tos wotstringstream
#define tfstream wifstream
#define tstring wstring
#define otstringstream wostringstream
#define itstringstream wistringstream
#define to_tstring to_wstring
#define tstringstream wstringstream
#else
#define tcout cout
#define tcin cin
#define tos otstringstream
#define tfstream ifstream
#define tstring string
#define otstringstream ostringstream
#define itstringstream istringstream
#define to_tstring to_string
#define tstringstream stringstream
#endif

typedef struct
{
	int actualX;
	int actualY;
	TCHAR map[MAX_TAM_MAP][MAX_TAM_MAP];
}Map;

typedef struct
{
	int pid;
	int numOfPlayers;
	char msg[BUFFER_SIZE];
	int scores[MAX_PLAYERS];
	Map map;
}Message;


#endif //MAIN_HEADER_H