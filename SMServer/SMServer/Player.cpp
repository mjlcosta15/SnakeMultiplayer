#include "Player.h"
#include "Game.h"


Player::Player(int pid, string playerName, Game * g) :
	name(playerName),
	points(0),
	game(g),
	hasGlue(false),
	hasOil(false),
	drunk(false),
	lost(false),
	isAutomated(false) {}

Player::Player(Game * g) :
	name("CPU Snake"),
	points(0),
	game(g),
	hasGlue(false),
	hasOil(false),
	drunk(false),
	lost(false),
	isAutomated(true) {}

Player::~Player()
{
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
	//se n�o houver mais blocos de cobra
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

	//Bater nas paredes
	if (snake.begin()->getPosX() < 0 || snake.begin()->getPosX() > game->getMapWidth() ||
		snake.begin()->getPosY() < 0 || snake.begin()->getPosY() > game->getMapHeight())
	{
		setLost(true);
		return;
	}

	Block * block = &game->getBlock(snake.begin()->getPosX(), snake.begin()->getPosY());

	switch (block->getBlockType()) {
	case EMPTY_BLOCK:
		//N�o fazer nada
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
			setOiled(true);
			game->changeBlock(*block);
		}
		break;
	case GLUE_BLOCK:
		//diminiu a velocidade
		if (!isAutomated) {
			setGlued(true);
			game->changeBlock(*block);
		}
		break;
	case O_VODKA_BLOCK:
		//inverte as teclas aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if (it->getPID() != pid)
					it->setDrunk(true);
			}
			game->changeBlock(*block);
		}
		break;
	case O_OIL_BLOCK:
		//aumenta a velocidade aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if (it->getPID() != pid)
					it->setOiled(true);
			}
			game->changeBlock(*block);
		}
		break;
	case O_GLUE_BLOCK:
		//diminiu a velocidade aos outros
		if (!isAutomated) {
			for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
				if (it->getPID() != pid)
					it->setGlued(true);
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
			if (it->hitByOtherSnake(block->getPosX(), block->getPosY())) {
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
	this->points += points;
}

void Player::removePoints(unsigned int points)
{
	this->points -= points;
}

string Player::getName() const
{
	return name;
}

void Player::setName(string name)
{
	this->name = name;
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
	if(this->direction == GOING_UP && direction != GOING_DOWN)
		this->direction = direction;
	else if(this->direction == GOING_DOWN && direction != GOING_UP)
		this->direction = direction;
	else if (this->direction == GOING_LEFT && direction != GOING_RIGHT)
		this->direction = direction;
	else if (this->direction == GOING_RIGHT && direction != GOING_LEFT)
		this->direction = direction;
}

bool Player::isGlued() const
{
	return hasGlue;
}

void Player::setGlued(bool glued)
{
	hasGlue = glued;
	if (hasGlue)
		setSnakeBlocksType(GLUED_SNAKE_BLOCK);
	else
		setSnakeBlocksType(SNAKE_BLOCK);
}

bool Player::isOiled() const
{
	return hasOil;
}

void Player::setOiled(bool oiled)
{
	hasOil = oiled;
	if (hasOil)
		setSnakeBlocksType(OILED_SNAKE_BLOCK);
	else
		setSnakeBlocksType(SNAKE_BLOCK);
}

bool Player::isDrunk() const
{
	return drunk;
}

void Player::setDrunk(bool drunk)
{
	this->drunk = drunk;
	if (this->drunk)
		setSnakeBlocksType(VODKA_SNAKE_BLOCK);
	else
		setSnakeBlocksType(SNAKE_BLOCK);
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
		//detetar cobra
		if (game->getBlock(x, y - 1).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_RIGHT);
		}//detetar parede acima
		else if (y - 1 < 0) {
			setDirection(GOING_RIGHT);
		}
		break;
	case GOING_DOWN:
		//detetar cobra
		if (game->getBlock(x, y + 1).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_RIGHT);
		}//detetar parede abaixo
		else if (y + 1 > game->getMapHeight()) {
			setDirection(GOING_LEFT);
		}
		break;
	case GOING_LEFT:
		//detetar cobra
		if (game->getBlock(x -1, y).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_UP);
		}//detetar parede � esquerda
		else if (x - 1 < 0) {
			setDirection(GOING_UP);
		}
		break;
	case GOING_RIGHT:
		//detetar cobra
		if (game->getBlock(x + 1, y).getBlockType() == SNAKE_BLOCK) {
			setDirection(GOING_DOWN);
		}//detetar parede � direita
		else if (x + 1 > game->getMapWidth()) {
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
	else if (p->isGlued())
		p->setGlued(false);
	else if (p->isOiled())
		p->setOiled(false);
	return 1;
}