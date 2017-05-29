#pragma once
#include "MainHeader.h"
class Player
{
	unsigned int pid;
	int points;
	string name;
	
public:
	
	Player(int pid, string name);
	~Player();

	int getPoints() const;
	string getName() const;
	int getPID() const;

};

