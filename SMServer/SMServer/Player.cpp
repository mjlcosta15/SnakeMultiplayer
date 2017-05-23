#include "Player.h"


Player::Player(string name, int pid) : playerPID(pid), playerName(name), points(0)
{
}

Player::~Player()
{
}

int Player::getPoints() const
{
	return points;
}

int Player::getPosX() const
{
	return posX;
}

int Player::getPosY() const
{
	return posY;
}

int Player::getSnakeSize() const
{
	return snakeSize;
}

string Player::getName()
{
	return playerName;
}

void Player::addPoints(unsigned int points)
{
	this->points += points;
}

void Player::removePoints(unsigned int points)
{
	this->points -= points;
}

void Player::setPosition(int x, int y)
{
	posX = x;
	posY = y;
}

void Player::decreaseSnakeSize(unsigned int num)
{
	snakeSize -= num;
}

void Player::increaseSnakeSize(unsigned int num)
{
	snakeSize += num;
}

void Player::setName(string name)
{
	playerName = name;
}

