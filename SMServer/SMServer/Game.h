#pragma once
#include "Player.h"
class Game
{
	vector<Player> players;
	int initialSnakeSize;
	int numSnakesAI;
	int mapWidth;
	int mapHeight;
	vector<vector<Block>> map;
	int gamePhase;

	const int INITIAL_PHASE = 1;
	const int IN_PROGRESS_PHASE = 2;
	const int FINISH_PHASE = 3;

public:
	Game();
	~Game();

	void addPlayer(Player newPlayer);
	void addPlayer(int pid, string name);

	bool remvovePlayer(Player player);
	
	int getNumSnakesAI() const;
	void setNumSnakesAI(int num);

	int getSnakeSize() const;
	void setSnakeSize(int size);

	int getGamePhase() const;
	void setInitalPhase();
	void setInProgressPhase();
	void setFinishPhase();

	void initMap();
	void updateMap();

	Message exportInfoToMessage();
};

