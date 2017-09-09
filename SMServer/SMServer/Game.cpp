#include "Game.h"



Game::Game()
{
	gamePhase = INITIAL_PHASE;
	idSnakeAI = 1;
	tick = 0;
	numPlayers = 1;
	playersInGame = 0;
}


Game::~Game()
{
	for (auto it = players.begin(); it != players.end(); it++)
		delete *it;
}

vector<Player *> Game::getPlayers()
{
	return players;
}

int Game::getNumPlayers() const
{
	return numPlayers;
}

void Game::setNumPlayers(int num)
{
	numPlayers = num;
}

void Game::addPlayer(Player *newPlayer)
{
	if (gamePhase == INITIAL_PHASE) {
		if (playersInGame < numPlayers) {
			players.push_back(newPlayer);
			playersInGame++;
		}
	}

}

void Game::addPlayer(int pid, string name)
{
	if (gamePhase == INITIAL_PHASE)
		players.push_back(new Player(pid, name, this));
}

bool Game::removePlayer(int pid)
{
	for (auto it = players.begin(); it != players.end(); it++)
		if ((*it)->getPID() == pid) {
			it = players.erase(it);
			playersInGame--;
			return true;
		}
	return false;
}

void Game::setDirectionToPlayer(int pid, int direction)
{
	for (auto it = players.begin(); it != players.end(); it++) {
		if ((*it)->getPID() == pid) {
			(*it)->setDirection(direction);
		}
	}
}

void Game::addSnakeAI(Player newPlayer)
{
	players.push_back(&newPlayer);
}

void Game::addSnakeAI(int id)
{
	players.push_back(new Player(id, this));
	idSnakeAI++;
}

void Game::addSnakeAIInGame(int id)
{
	players.push_back(new Player(id, this));
	idSnakeAI++;
}

bool Game::removeSnakeAI(Player player)
{
	return false;
}

void Game::setNumberOfObjects(unsigned int num)
{
	numberOfObjects = num;
}

int Game::getNumberOfObjects() const
{
	return numberOfObjects;
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
		objectType = rand() % COFFEE_BLOCK + 1;
		do {
			x = rand() % mapWidth;
			y = rand() % mapHeight;
		} while (map.at(y).at(x).getBlockType() != EMPTY_BLOCK);
		map.at(x).at(y).setBlockType(objectType);
	}

	for (int i = 0; i < numSnakesAI; i++) {
		addSnakeAI(idSnakeAI);

	}

	//make the snakes
	for (auto it = players.begin(); it != players.end(); it++) {
		x = rand() % mapWidth + 2;
		y = rand() % mapHeight + 2;
		(*it)->initSnake(y, x);
	}

}

void Game::updateMap()
{
	tick++;

	for (auto it = players.begin(); it != players.end(); it++) {
		if ((*it)->getSpeed() == tick) {
			cout << "[" << (*it)->getName() << "] Vou mexer-me com velocidade " << tick << endl;
			(*it)->moveSnake();
			(*it)->effectAfterMovement();
		}

	}
	if (tick == SPEED_SLOW)
		tick = 0;
}

void Game::changeBlock(Block block)
{
	map.at(block.getPosX()).at(block.getPosY()).setBlockType(EMPTY_BLOCK);
	addSpecialBlock();
}

void Game::addSpecialBlock()
{
	int objectType = rand() % COFFEE_BLOCK + 1;
	int x, y;
	do {
		x = rand() % mapWidth;
		y = rand() % mapHeight;
	} while (map.at(y).at(x).getBlockType() != EMPTY_BLOCK);

	map.at(y).at(x).setBlockType(objectType);
}

void Game::addSpecialBlock(int type, int number)
{
	int x, y;
	for (int i = 0; i < number; i++) {
		do {
			x = rand() % mapWidth;
			y = rand() % mapHeight;
		} while (map.at(y).at(x).getBlockType() != EMPTY_BLOCK);

		map.at(y).at(x).setBlockType(type);
	}
}

Block Game::getBlock(int x, int y)
{
	return map.at(y).at(x);
}

Message Game::exportInfoToMessage()
{
	Message msg;
	msg.code = MAP;
	msg.map.actualX = mapWidth;
	msg.map.actualY = mapHeight;

	msg.numOfPlayers = players.size();

	//fill the map
	if (gamePhase != INITIAL_PHASE) {
		for (int i = 0; i < MAX_TAM_MAP; i++) {
			for (int j = 0; j < MAX_TAM_MAP; j++) {
				if (i < msg.map.actualX && j < msg.map.actualY) {
					msg.map.map[i][j] = map.at(i).at(j).getCharId();
				}
				else
					msg.map.map[i][j] = '-';

			}

		}
		for (auto it = players.begin(); it != players.end(); it++) {
			vector<Block> temp = (*it)->getSnake();
			for (auto its = temp.begin(); its != temp.end(); its++)
				msg.map.map[its->getPosX()][its->getPosY()] = its->getCharId();
		}

		for (int i = 0; i < mapWidth; i++) {
			for (int j = 0; j < mapHeight; j++) {
				cout << " " << msg.map.map[i][j] << " ";
			}
			if (i < mapHeight)
				cout << endl;
		}

		//fill the scoresx
		for (auto it = players.begin(); it != players.end(); it++)
			msg.scores[distance(players.begin(), it)] = (*it)->getPoints();
	}

	return msg;
}
