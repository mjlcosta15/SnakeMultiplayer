#include "Player.h"
#include "Game.h"


Player::Player(int pid, string playerName, Game * g) :
	name(playerName),
	points(0),
	game(g),
	hasGlue(false),
	hasOil(false),
	drunk(false) {}

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
}

 
void Player::moveSnake()
{
	int coord, y, x;
	switch (direction) {
	case GOING_UP:
		coord = snake.front().getPosY() - 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			y = it->getPosY();
			x = it->getPosX();
			it->setBlockPosition(x, coord);
			coord = y;
		}
		break;
	case GOING_DOWN:
		coord = snake.front().getPosY() + 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			y = it->getPosY();
			x = it->getPosX();
			it->setBlockPosition(x, coord);
			coord = y;
		}
		break;
	case GOING_LEFT:
		coord = snake.front().getPosX() - 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			x = it->getPosX();
			y = it->getPosY();
			it->setBlockPosition(coord, y);
			coord = x;
		}
		break;
	case GOING_RIGHT:
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
		setDrunk(true);
		game->changeBlock(*block);
		break;
	case GRANADE_BLOCK:
		//perde
		game->changeBlock(*block);
		break;
	case OIL_BLOCK:
		//aumenta a velocidade
		setOiled(true);
		game->changeBlock(*block);
		break;
	case GLUE_BLOCK:
		//diminiu a velocidade
		setGlued(true);
		game->changeBlock(*block);
		break;
	case O_VODKA_BLOCK:
		//inverte as teclas aos outros
		for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
			if (it->getPID() != pid)
				it->setDrunk(true);
		}
		game->changeBlock(*block);
		break;
	case O_OIL_BLOCK:
		//aumenta a velocidade aos outros
		for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
			if (it->getPID() != pid)
				it->setOiled(true);
		}
		game->changeBlock(*block);
		break;
	case O_GLUE_BLOCK:
		//diminiu a velocidade aos outros
		for (auto it = game->getPlayers().begin(); it != game->getPlayers().end(); it++) {
			if (it->getPID() != pid)
				it->setGlued(true);
		}
		game->changeBlock(*block);
		break;
	case SNAKE_BLOCK:
		//perde
		break;

	default:
		break;
	}
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

DWORD WINAPI countEffect(LPVOID param) {

	Player * p = (Player *)param;

	Sleep(10000);

	if (p->isDrunk())
		p->setDrunk(false);
	else if (p->isGlued())
		p->setGlued(false);
	else if (p->isOiled())
		p->setOiled(false);
}