#pragma once
#include "Block.h"

class Player;

class SnakeBlock :
	public Block
{
	Player * player;
public:
	SnakeBlock(Player p);
	~SnakeBlock();
	void effect();
};

