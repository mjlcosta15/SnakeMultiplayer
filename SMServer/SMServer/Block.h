#pragma once
class Block
{
	int posX;
	int posY;
public:
	Block();
	~Block();

	virtual char getCharId() = 0;
	virtual void effect() = 0;
	int getPosX() const;
	int getPosY() const;
	void setBlockPosition(int x, int y);
};

