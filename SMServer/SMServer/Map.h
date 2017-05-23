#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#define MAPSIZE 524

using namespace std;

class Map {
	ifstream File;
	vector<char> map;
public:
	
	Map(string filePath);
	~Map();
	void printMap();
	void loadFromFile(string filePath);



};
