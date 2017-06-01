#include "Game.h"



Game::Game()
{
	gamePhase = INITIAL_PHASE;
}


Game::~Game()
{
}

void Game::addPlayer(Player newPlayer)
{
	players.push_back(newPlayer);
}

void Game::addPlayer(int pid, string name)
{
	players.push_back(Player(pid, name, this));
}

bool Game::remvovePlayer(Player player)
{
	for (auto it = players.begin(); it != players.end(); it++)
		if (it->getPID() == player.getPID()) {
			it = players.erase(it);
			return true;
		}
	return false;
}

int Game::getNumSnakesAI() const
{
	return numSnakesAI;
}

void Game::setNumSnakesAI(int num)
{
	numSnakesAI = num;
}

int Game::getSnakeSize() const
{
	return initialSnakeSize;
}

void Game::setSnakeSize(int size)
{
	if (size >= 1)
		initialSnakeSize = size;
	else
		initialSnakeSize = 1;
}

int Game::getGamePhase() const
{
	return gamePhase;
}

void Game::setInitalPhase()
{
	gamePhase = INITIAL_PHASE;
}

void Game::setInProgressPhase()
{
	gamePhase = IN_PROGRESS_PHASE;
}

void Game::setFinishPhase()
{
	gamePhase = FINISH_PHASE;
}

void Game::initMap()
{
	//Init map with empty blocks
	for (int i = 0; i < mapHeight; i++) {
		vector<Block> temp;
		for (int j = 0; j < mapWidth; j++) {
			temp.push_back(Block(i, j, EMPTY_BLOCK));
		}
		map.push_back(temp);
	}

	//fill with some objects
}

void Game::updateMap()
{
}

Message Game::exportInfoToMessage()
{
	Message msg;
	msg.map.actualX = map.size();
	msg.map.actualY = map.at(0).size();

	//fill the map
	for (int i = 0; i < MAX_TAM_MAP; i++) {
		for (int j = 0; j < MAX_TAM_MAP; j++) {
			if (i < msg.map.actualX && j < msg.map.actualY)
				msg.map.map[i][j] = map.at(i).at(j).getCharId();
			else
				msg.map.map[i][j] = '-';
		}
	}

	//fill the scores
	for (auto it = players.begin(); it != players.end(); it++)
		msg.scores[distance(players.begin(), it)] = it->getPoints();

	return msg;
}
