#include "Block.h"



Block::Block(int x, int y, int type) : posX(x), posY(y), blockType(type)
{
}


Block::~Block()
{
}


int Block::getPosX() const
{
	return posX;
}

void Block::setPosX(int x)
{
	if (x < 0 || x > 80)
		return;

	posX = x;

}

int Block::getPosY() const
{
	return posY;
}

void Block::setPosY(int y)
{
	if (y < 0 || y > 80)
		return;
	posY = y;
}

int Block::getBlockType() const
{
	return blockType;
}

char Block::getCharId()
{
	switch (blockType)
	{
	case EMPTY_BLOCK:
		return '_';
		break;
	case ICE_BLOCK:
		return 'i';
		break;
	case FOOD_BLOCK:
		return 'f';
		break;
	case VODKA_BLOCK:
		return 'v';
		break;
	case GRANADE_BLOCK:
		return 'b';
		break;
	case OIL_BLOCK:
		return 'o';
		break;
	case GLUE_BLOCK:
		return 'g';
		break;
	case O_VODKA_BLOCK:
		return 'V';
		break;
	case O_OIL_BLOCK:
		return 'O';
		break;
	case O_GLUE_BLOCK:
		return 'G';
		break;
	case SNAKE_BLOCK:
		return 's';
		break;
	default:
		return '?';
		break;
	}
}

void Block::setBlockPosition(int x, int y)
{
	if (x < 0 || x > 80 || y < 0 || y > 80)
		return;

	posX = x;
	posY = y;
}
