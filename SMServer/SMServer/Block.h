#pragma once

#define EMPTY_BLOCK 1
#define ICE_BLOCK  2
#define FOOD_BLOCK  3
#define VODKA_BLOCK  4
#define GRANADE_BLOCK  5
#define OIL_BLOCK  6
#define GLUE_BLOCK  7
#define O_VODKA_BLOCK  8
#define O_OIL_BLOCK  9
#define O_GLUE_BLOCK  10
#define SNAKE_BLOCK  11

class Block
{
	int posX;
	int posY;
	int blockType;


public:

	Block(int x, int y, int type);
	~Block();

	int getPosX() const;
	void setPosX(int x);
	int getPosY() const;
	void setPosY(int y);
	void setBlockPosition(int x, int y);
	
	int getBlockType() const;
	char getCharId();
	
	
};

