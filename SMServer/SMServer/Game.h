#ifndef GAME_H
#define GAME_H

#include "Player.h"

#define INITIAL_PHASE 1
#define IN_PROGRESS_PHASE 2
#define FINISH_PHASE 3

class Game
{
	vector<Player *> players;
	int initialSnakeSize;
	int numPlayers;
	int playersInGame;
	int numSnakesAI;
	int idSnakeAI;
	int mapWidth;
	int mapHeight;
	int numberOfObjects;
	vector<vector<Block>> map;
	int gamePhase;
	int tick;

public:
	Game();
	~Game();

	vector<Player *> getPlayers();

	int getNumPlayers()const;
	void setNumPlayers(int num);

	void addPlayer(Player * newPlayer);
	void addPlayer(int pid, string name);
	bool removePlayer(int pid);
	void setDirectionToPlayer(int pid, int direction);


	void addSnakeAI(Player newPlayer);
	void addSnakeAI(int id);
	void addSnakeAIInGame(int id);

	bool removeSnakeAI(Player player);

	void setNumberOfObjects(unsigned int num);
	int getNumberOfObjects() const;

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
	void addSpecialBlock(int type, int number);
	Block getBlock(int x, int y);


	Message exportInfoToMessage();
};

#endif