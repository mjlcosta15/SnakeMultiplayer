#ifndef GAME_H
#define GAME_H

#include "Player.h"

#define INITIAL_PHASE 1
#define IN_PROGRESS_PHASE 2
#define FINISH_PHASE 3

class Game
{
	vector<Player> players;
	vector<Player> snakesAI;
	int initialSnakeSize;
	int numSnakesAI;
	int mapWidth;
	int mapHeight;
	int numberOfObjects;
	vector<vector<Block>> map;
	int gamePhase;



public:
	Game();
	~Game();

	vector<Player> getPlayers();

	void addPlayer(Player newPlayer);
	void addPlayer(int pid, string name);

	bool removePlayer(Player player);
	
	vector<Player> getSnakeAIs();

	void addSnakeAI(Player newPlayer);
	void addSnakeAI();
	void addSnakeAIInGame();

	bool removeSnakeAI(Player player);


	int getNumSnakesAI() const;
	void setNumSnakesAI(int num);

	int getSnakeSize() const;
	void setSnakeSize(int size);

	int getGamePhase() const;
	void setInitalPhase();
	void setInProgressPhase();
	void setFinishPhase();

	int getMapWidth() const;
	void setMapWidth(int width);

	int getMapHeight() const;
	void setMapHeight(int height);

	void initMap();
	void updateMap();
	void changeBlock(Block block);
	void addSpecialBlock();
	Block getBlock(int x, int y);


	Message exportInfoToMessage();
};

#endif