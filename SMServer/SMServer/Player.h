#ifndef PLAYER_H
#define PLAYER_H

#include "MainHeader.h"
#include "Block.h"

#define GOING_UP 1
#define GOING_DOWN 2
#define GOING_LEFT 3
#define GOING_RIGHT 4

class Game;

class Player
{
	unsigned int pid;
	int points;
	string name;
	vector<Block> snake;
	Game * game;
	int direction;

public:

	Player(int pid, string name, Game * g);
	~Player();
	 
	void initSnake(int posX, int posY);
	void increaseSnake(unsigned int numBlocks);
	void decreaseSnake(unsigned int numBlocks);
	void moveSnake();
	
	void effectAfterMovement(Block block);

	int getPoints() const;
	void addPoints(unsigned int points);
	void removePoints(unsigned int points);


	string getName() const;
	void setName(string name);
	
	int getPID() const;

	int getDirection() const;
	void setDirection(unsigned int direction);

};

#endif //PLAYER_H