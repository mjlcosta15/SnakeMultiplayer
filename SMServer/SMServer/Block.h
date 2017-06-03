#ifndef BLOCK_H
#define BLOCK_H

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
#define OILED_SNAKE_BLOCK  12
#define GLUED_SNAKE_BLOCK  13
#define VODKA_SNAKE_BLOCK  14

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

	//Change the block to the position (x,y)
	void setBlockPosition(int x, int y);
	
	int getBlockType() const;
	void setBlockType(int type);
	
	//Get the char corresponding the type of the block
	char getCharId();
	
	
};

#endif