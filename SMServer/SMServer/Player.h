#pragma once
#include "MainHeader.h"

class SnakeBlock;

class Player
{
	unsigned int pid;
	int points;
	string name;
	vector<SnakeBlock> snake;
	
public:
	
	Player(int pid, string name);
	~Player();

	void initSnake(int posX, int posY);

	int getPoints() const;
	string getName() const;
	int getPID() const;

};

