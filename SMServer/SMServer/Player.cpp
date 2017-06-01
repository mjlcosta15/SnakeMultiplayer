#include "Player.h"
#include "Game.h"


Player::Player(int pid, string playerName, Game * g) : name(playerName), points(0), game(g)
{
}

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

void Player::increaseSnake(int numBlocks)
{
	int x, y;
	for (int i = 0; i < numBlocks; i++) {
		x = snake.back().getPosX() + 1;
		y = snake.back().getPosY();
		snake.push_back(Block(x, y, SNAKE_BLOCK));
	}
}

void Player::decreaseSnake(int numBlocks)
{
	for (int i = 0; i < numBlocks; i++)
		snake.pop_back();
}

void Player::moveSnake()
{
	int coord, temp;
	switch (direction)
	{
	case GOING_UP:
		coord = snake.front().getPosY() - 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			temp = it->getPosY();
			it->setPosY(coord);
			coord = temp;
		}
		break;
	case GOING_DOWN:
		coord = snake.front().getPosY() + 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			temp = it->getPosY();
			it->setPosY(coord);
			coord = temp;
		}
		break;
	case GOING_LEFT:
		coord = snake.front().getPosX() - 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			temp = it->getPosX();
			it->setPosX(coord);
			coord = temp;
		}
		break;
	case GOING_RIGHT:
		coord = snake.front().getPosX() + 1;
		for (auto it = snake.begin(); it != snake.end(); it++) {
			temp = it->getPosX();
			it->setPosX(coord);
			coord = temp;
		}
		break;
	default:
		break;
	}

}

void Player::effectAfterMovement(Block block)
{
	switch (block.getBlockType()) {
	case EMPTY_BLOCK:
		//Mover para esse bloco
		moveSnake();
		break;
	case ICE_BLOCK:
		//perde um segmento e 2pts
		break;
	case FOOD_BLOCK:
		//aumenta um segmento e 2pts
		break;
	case VODKA_BLOCK:
		//inverter as teclas
		break;
	case GRANADE_BLOCK:
		//perde
		break;
	case OIL_BLOCK:
		//aumenta a velocidade
		break;
	case GLUE_BLOCK:
		//diminiu a velocidade
		break;
	case O_VODKA_BLOCK:
		//inverte as teclas aos outros
		break;
	case O_OIL_BLOCK:
		//aumenta a velocidade aos outros
		break;
	case O_GLUE_BLOCK:
		//diminiu a velocidade aos outros
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
	return 0;
}

string Player::getName() const
{
	return string();
}

int Player::getPID() const
{
	return 0;
}
