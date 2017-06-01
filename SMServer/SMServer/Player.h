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

	void increaseSnake(int numBlocks);
	void decreaseSnake(int numBlocks);
	void moveSnake();
	void effectAfterMovement(Block block);

	int getPoints() const;
	string getName() const;
	int getPID() const;

};

#endif //PLAYER_H