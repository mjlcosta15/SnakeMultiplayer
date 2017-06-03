#include "Game.h"



Game::Game()
{
	gamePhase = INITIAL_PHASE;
}


Game::~Game()
{
}

vector<Player> Game::getPlayers()
{
	return players;
}

void Game::addPlayer(Player newPlayer)
{
	if (gamePhase == INITIAL_PHASE)
		players.push_back(newPlayer);
}

void Game::addPlayer(int pid, string name)
{
	if (gamePhase == INITIAL_PHASE)
		players.push_back(Player(pid, name, this));
}

bool Game::removePlayer(Player player)
{
	for (auto it = players.begin(); it != players.end(); it++)
		if (it->getPID() == player.getPID()) {
			it = players.erase(it);
			return true;
		}
	return false;
}

vector<Player> Game::getSnakeAIs()
{
	return vector<Player>();
}

void Game::addSnakeAI(Player newPlayer)
{
	snakesAI.push_back(newPlayer);
}

void Game::addSnakeAI()
{
	snakesAI.push_back(Player(this));
}

void Game::addSnakeAIInGame()
{
	snakesAI.push_back(Player(this));
}

bool Game::removeSnakeAI(Player player)
{
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

int Game::getMapWidth() const
{
	return mapWidth;
}

void Game::setMapWidth(int width)
{
	if (width < 0 || width > MAX_TAM_MAP)
		return;

	mapWidth = width;
}

int Game::getMapHeight() const
{
	return mapHeight;
}

void Game::setMapHeight(int height)
{
	if (height < 0 || height > MAX_TAM_MAP)
		return;

	mapHeight = height;
}

void Game::initMap()
{
	//init map with empty blocks
	for (int i = 0; i < mapHeight; i++) {
		vector<Block> temp;
		for (int j = 0; j < mapWidth; j++) {
			temp.push_back(Block(i, j, EMPTY_BLOCK));
		}
		map.push_back(temp);
	}

	//fill with some objects
	int objectType, x, y;
	for (int i = 0; i < numberOfObjects; i++) {
		objectType = rand() % O_GLUE_BLOCK + 1;
		do {
			x = rand() % mapWidth;
			y = rand() % mapHeight;
		} while (map.at(x).at(y).getBlockType() != EMPTY_BLOCK);
		map.at(x).at(y).setBlockType(objectType);
	}

	//make the snakes
	for (auto it = players.begin(); it != players.end(); it++) {
		x = rand() % mapWidth + 2;
		y = rand() % mapHeight + 2;
		it->initSnake(x, y);
	}

}

void Game::updateMap()
{
	Sleep(250);
	for (auto it = players.begin(); it != players.end(); it++) {
		if (it->isOiled()) {
			it->moveSnake();
			it->effectAfterMovement();
		}
	}
	Sleep(250);
	for (auto it = players.begin(); it != players.end(); it++) {
		if (!it->isOiled() && !it->isGlued()) {
			it->moveSnake();
			it->effectAfterMovement();
		}
	}
	Sleep(250);
	for (auto it = players.begin(); it != players.end(); it++) {
		if (it->isGlued()) {
			it->moveSnake();
			it->effectAfterMovement();
		}
	}
}

void Game::changeBlock(Block block)
{
	map.at(block.getPosX()).at(block.getPosY()).setBlockType(EMPTY_BLOCK);
	addSpecialBlock();
}

void Game::addSpecialBlock()
{
	int objectType = rand() % O_GLUE_BLOCK + 1;
	int x, y;
	do {
		x = rand() % mapWidth;
		y = rand() % mapHeight;
	} while (map.at(x).at(y).getBlockType() != EMPTY_BLOCK);

	map.at(x).at(y).setBlockType(objectType);
}

Block Game::getBlock(int x, int y)
{
	return map.at(x).at(y);
}

Message Game::exportInfoToMessage()
{
	Message msg;
	msg.map.actualX = mapWidth;
	msg.map.actualY = mapHeight;

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
