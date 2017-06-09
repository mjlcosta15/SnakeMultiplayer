#include "Player.h"
#include "Game.h"


HANDLE Player::hMutex = NULL;


Player::Player(int pid, string playerName, Game * g) :
	name(playerName),
	points(0),
	game(g),
	speed(SPEED_NORMAL),
	hasGlue(false),
	hasOil(false),
	drunk(false),
	lost(false),
	isAutomated(false),
	direction(GOING_LEFT)
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
}

Player::Player(int id, Game * g) :
	pid(id),
	name("CPU Snake"),
	points(0),
	game(g),
	speed(SPEED_NORMAL),
	hasGlue(false),
	hasOil(false),
	drunk(false),
	lost(false),
	isAutomated(true),
	direction(GOING_LEFT)
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
}

Player::~Player()
{
	CloseHandle(hMutex);
	delete game;
}

void Player::initSnake(int posX, int posY)
{
	for (int i = 0; i < game->getSnakeSize(); i++) {
		snake.push_back(Block(posX + i, posY, SNAKE_BLOCK));
	}
}

void Player::increaseSnake(unsigned int numBlocks)
{
	int x, y;
	for (unsigned int i = 0; i < numBlocks; i++) {
		x = snake.back().getPosX() + 1;
		y = snake.back().getPosY();
		snake.push_back(Block(x, y, SNAKE_BLOCK));
	}
}

void Player::decreaseSnake(unsigned int numBlocks)
{
	for (unsigned int i = 0; i < numBlocks; i++)
		snake.pop_back();
	//se não houver mais blocos de cobra
	if (snake.empty())
		setLost(true);
}


void Player::moveSnake()
{
	if (lost)
		return;

	if (isAutomated)
		detectObstacle();

	int coord, y, x;
	switch (direction) {
	case GOING_UP:
		if (drunk)
			coord = snake.front().getPosY() + 1;
		else
			coord = snake.front().getPosY() - 1;

		for (auto it = snake.begin(); it != snake.end(); it++) {
			y = it->getPosY();
			x = it->getPosX();
			it->setBlockPosition(x, coord);
			coord = y;
		}
		break;
	case GOING_DOWN:
		if (drunk)
			coord = snake.front().getPosY() - 1;
		else
			coord = snake.front().getPosY() + 1;

		for (auto it = snake.begin(); it != snake.end(); it++) {
			y = it->getPosY();
			x = it->getPosX();
			it->setBlockPosition(x, coord);
			coord = y;
		}
		break;
	case GOING_LEFT:
		if (drunk)
			coord = snake.front().getPosX() + 1;
		else
			coord = snake.front().getPosX() - 1;

		for (auto it = snake.begin(); it != snake.end(); it++) {
			x = it->getPosX();
			y = it->getPosY();
			it->setBlockPosition(coord, y);
			coord = x;
		}
		break;
	case GOING_RIGHT:
		if (drunk)
			coord = snake.front().getPosX() - 1;
		else
			coord = snake.front().getPosX() + 1;

		for (auto it = snake.begin(); it != snake.end(); it++) {
			x = it->getPosX();
			y = it->getPosY();
			it->setBlockPosition(coord, y);
			coord = x;
		}
		break;
	default:
		break;
	}
}

vector<Block> Player::getSnake() const
{
	return snake;
}


void Player::setSnakeBlocksType(int type)
{
	for (auto it = snake.begin(); it != snake.end(); it++) {
		it->setBlockType(type);
	}
}


void Player::effectAfterMovement()
{
	if (lost)
		return;
	int x = snake.begin()->getPosX();
	int y = snake.begin()->getPosY();
	//Bater nas paredes
	if (snake.begin()->getPosX() < 0 || snake.begin()->getPosX() >= game->getMapWidth())
	{
		setLost(true);
		return;
	}
	if (snake.begin()->getPosY() < 0 || snake.begin()->getPosY() >= game->getMapHeight())
	{
		setLost(true);
		return;
	}

	Block * block = &game->getBlock(snake.begin()->getPosX(), snake.begin()->getPosY());

	switch (block->getBlockType()) {
	case EMPTY_BLOCK:
		//Não fazer nada
		break;
	case ICE_BLOCK:
		//perde um segmento e 2pts
		decreaseSnake(1);
		removePoints(2);
		game->changeBlock(*block);
		break;
	case FOOD_BLOCK:
		//aumenta um segmento e 2pts
		increaseSnake(1);
		addPoints(2);
		game->changeBlock(*block);
		break;
	case VODKA_BLOCK:
		//inverter as teclas
		if (!isAutomated) {
			setDrunk(true);
			game->changeBlock(*block);
		}
		break;
	case GRANADE_BLOCK:
		//perde
		if (!isAutomated) {
			setLost(true);
			game->changeBlock(*block);
		}
		break;
	case OIL_BLOCK:
		//aumenta a velocidade
		if (!isAutomated) {
			setSpeed(SPEED_FAST);
			game->changeBlock(*block);
		}
		break;
	case GLUE_BLOCK:
		//diminiu a velocidade
		if (!isAutomated) {
			setSpeed(SPEED_SLOW);
			game->changeBlock(*block);
		}
		break;
	case O_VODKA_BLOCK:
		//inverte as teclas aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if ((*it)->getPID() != pid)
					(*it)->setDrunk(true);
			}
			game->changeBlock(*block);
		}
		break;
	case O_OIL_BLOCK:
		//aumenta a velocidade aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if ((*it)->getPID() != pid)
					(*it)->setSpeed(SPEED_FAST);
			}
			game->changeBlock(*block);
		}
		break;
	case O_GLUE_BLOCK:
		//diminiu a velocidade aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if ((*it)->getPID() != pid)
					(*it)->setSpeed(SPEED_SLOW);
			}
			game->changeBlock(*block);
		}
		break;
	case SNAKE_BLOCK:
		//perde
		for (auto it = snake.begin(); it != snake.end(); it++) {
			if (it->getPosX() == block->getPosX() && it->getPosY() == block->getPosY()) {
				setLost(true);
				break;
			}
		}
		for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
			if ((*it)->hitByOtherSnake(block->getPosX(), block->getPosY())) {
				setLost(true);
			}
		}

		break;

	default:
		break;
	}
}

bool Player::hitByOtherSnake(int x, int y)
{
	for (auto it = snake.begin(); it != snake.end(); it++) {
		if (it->getPosX() == x && it->getPosY() == y) {
			addPoints(10);
			return true;
		}
	}
	return false;
}

int Player::getPoints() const
{
	return points;
}

void Player::addPoints(unsigned int points)
{
	WaitForSingleObject(hMutex, INFINITE);

	this->points += points;

	ReleaseMutex(hMutex);
}

void Player::removePoints(unsigned int points)
{
	WaitForSingleObject(hMutex, INFINITE);

	this->points -= points;

	ReleaseMutex(hMutex);
}

string Player::getName() const
{
	return name;
}

void Player::setName(string name)
{
	WaitForSingleObject(hMutex, INFINITE);

	this->name = name;

	ReleaseMutex(hMutex);
}

int Player::getPID() const
{
	return pid;
}

int Player::getDirection() const
{
	return direction;
}

void Player::setDirection(unsigned int direction)
{
	if (direction <= 0 && direction > GOING_RIGHT)
		return;

	WaitForSingleObject(hMutex, INFINITE);

	if (this->direction == GOING_UP && direction != GOING_DOWN)
		this->direction = direction;
	else if (this->direction == GOING_DOWN && direction != GOING_UP)
		this->direction = direction;
	else if (this->direction == GOING_LEFT && direction != GOING_RIGHT)
		this->direction = direction;
	else if (this->direction == GOING_RIGHT && direction != GOING_LEFT)
		this->direction = direction;

	ReleaseMutex(hMutex);
}

int Player::getSpeed() const
{
	return speed;
}

void Player::setSpeed(int speed)
{
	WaitForSingleObject(hMutex, INFINITE);

	if (speed == SPEED_SLOW) {
		this->speed = speed;
		setSnakeBlocksType(GLUED_SNAKE_BLOCK);
	}
	else if (speed == SPEED_NORMAL) {
		this->speed = speed;
		setSnakeBlocksType(SNAKE_BLOCK);
	}
	else if (speed == SPEED_FAST) {
		this->speed = speed;
		setSnakeBlocksType(OILED_SNAKE_BLOCK);
	}

	ReleaseMutex(hMutex);
}


bool Player::isDrunk() const
{
	return drunk;
}

void Player::setDrunk(bool drunk)
{
	WaitForSingleObject(hMutex, INFINITE);

	this->drunk = drunk;
	if (this->drunk)
		setSnakeBlocksType(VODKA_SNAKE_BLOCK);
	else
		setSnakeBlocksType(SNAKE_BLOCK);

	ReleaseMutex(hMutex);
}

bool Player::isLost() const
{
	return lost;
}

void Player::setLost(bool lost)
{
	this->lost = lost;
}

void Player::detectObstacle()
{
	int x = snake.front().getPosX();
	int y = snake.front().getPosY();
	switch (direction) {
	case GOING_UP:
		//detetar parede acima
		if (y - 1 < 0) {
			setDirection(GOING_RIGHT);
		}//detetar cobra
		else if (game->getBlock(x, y - 1).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_RIGHT);
		}
		break;
	case GOING_DOWN:
		//detetar parede abaixo
		if (y + 1 > game->getMapHeight()) {
			setDirection(GOING_LEFT);
		}//detetar cobra
		else if (game->getBlock(x, y + 1).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_RIGHT);
		}
		break;
	case GOING_LEFT:

		//detetar parede à esquerda
		if (x - 1 < 0) {
			setDirection(GOING_UP);
		}//detetar cobra
		else if (game->getBlock(x - 1, y).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_UP);
		}
		break;
	case GOING_RIGHT:
		//detetar parede à direita
		if (x + 1 > game->getMapWidth()) {
			setDirection(GOING_DOWN);
		}//detetar cobra
		else if (game->getBlock(x + 1, y).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_DOWN);
		}
		break;
	}
}

DWORD WINAPI countEffect(LPVOID param) {

	Player * p = (Player *)param;

	Sleep(10000);

	if (p->isDrunk())
		p->setDrunk(false);
	else if (p->getSpeed() == SPEED_SLOW)
		p->setSpeed(SPEED_NORMAL);
	else if (p->getSpeed() == SPEED_FAST)
		p->setSpeed(SPEED_NORMAL);

	return 1;
}