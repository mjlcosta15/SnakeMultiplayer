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
	bool hasGlue;
	bool hasOil;
	bool drunk;
	bool lost;
	//AI Snakes
	bool isAutomated;

public:
	//Constructor for Player
	Player(int pid, string name, Game * g);
	//Constructor for AI snake
	Player(Game * g);
	
	~Player();
	
	//Create the snake in the position (x,y)
	void initSnake(int posX, int posY);
	//Add N blocks to the snake
	void increaseSnake(unsigned int numBlocks);
	//Deletes N blocks on the snake
	void decreaseSnake(unsigned int numBlocks);
	//Moves the snake according with the chosed direction
	void moveSnake();
	
	//Changes the blocks type of the snake
	void setSnakeBlocksType(int type);

	//Makes the consequence of the snake's movement
	void effectAfterMovement();

	//Check if the snake was hit by another snake
	bool hitByOtherSnake(int x, int y);

	int getPoints() const;
	//Add N points to the score
	void addPoints(unsigned int points);
	//Remove N points to the score
	void removePoints(unsigned int points);


	string getName() const;
	void setName(string name);
	
	int getPID() const;

	int getDirection() const;
	void setDirection(unsigned int direction);

	bool isGlued() const;
	void setGlued(bool glued);

	bool isOiled() const;
	void setOiled(bool oiled);

	bool isDrunk() const;
	void setDrunk(bool drunk);

	bool isLost()const;
	void setLost(bool lost);

	void detectObstacle();
};

#endif //PLAYER_H