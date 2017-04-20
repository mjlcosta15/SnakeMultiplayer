#pragma once

#include <iostream>
#include <fstream>

using namespace std;

class Map {

public:
	Map(string filePath);
	
private:
	ifstream File;
	void loadFromFile(string filePath);

};
