#include "CreateDLL.h"

static const TCHAR szName[] = TEXT("SnakeMultiplayerSharedMem");

CreateDLL::CreateDLL()
{
}

CreateDLL::~CreateDLL()
{
}


SNAKE_MULTIPLAYER_API bool CreateDLL::ReadFromSharedMemoryBuffer()
{
	return false;
}

SNAKE_MULTIPLAYER_API bool CreateDLL::WriteToSharedMemoryBuffer()
{
	return false;
}

