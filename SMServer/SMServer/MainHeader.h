#pragma once
#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <sstream>
#include <process.h>
#include <io.h>

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

#define BUFFERSIZE 2048
#define MAX_TAM_MAP 80

struct Map
{
	int maxX;
	int maxY;
	char map[MAX_TAM_MAP][MAX_TAM_MAP];
};

struct Message
{
	int pid;
	char msg[BUFFERSIZE];
	Map map;
};

#endif //MAIN_HEADER_H