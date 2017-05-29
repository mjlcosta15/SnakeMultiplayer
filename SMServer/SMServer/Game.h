#pragma once
#include "Player.h"
#include "Block.h"
class Game
{
	vector<Player> players;
	int initialSnakeSize;
	int numSnakesAI;
	vector<vector<Block>> map;

public:
	Game();
	~Game();

	Message exportInfoToMessage();
};

