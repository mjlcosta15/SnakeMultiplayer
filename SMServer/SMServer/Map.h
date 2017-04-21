#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#define MAPSIZE 524

using namespace std;

class Map {

public:
	Map(string filePath);
	void printMap();
	
private:
	ifstream File;
	vector<char> map;

	void loadFromFile(string filePath);

};
