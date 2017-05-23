#pragma once
#include "MainHeader.h"

class Player
{
	int playerPID;
	string playerName;
	int points;
	int posX;
	int posY;
	int snakeSize;
public:
	Player(string name, int pid);
	~Player();

	int getPoints() const;
	int getPosX() const;
	int getPosY() const;
	int getSnakeSize() const;
	string getName();

	void addPoints(unsigned int points);
	void removePoints(unsigned int points);
	void setPosition(int x, int y);
	void decreaseSnakeSize(unsigned int num);
	void increaseSnakeSize(unsigned int num);
	void setName(string name);

};

