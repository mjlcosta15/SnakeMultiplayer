#include "Game.h"



Game::Game()
{
}


Game::~Game()
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
