#include "Map.h"

Map::Map(string filePath) {

	loadFromFile(filePath);
}

Map::~Map()
{
}

void Map::printMap() {

	int pos = 0;

	while (pos!=MAPSIZE) {
		cout << this->map[pos];
		pos++;
	}

	cout << endl;

}

void Map::loadFromFile(string filePath) {

	char c;
	ifstream is(filePath);

	File.open(filePath); // Open file

	if (!File) {		// If the file does not exist
		cout << "The file does not exist!" << endl;
		exit(1);
	}


	while (is.get(c)) {
		//cout << c;
		this->map.push_back(c);
	}
	

	is.close();

}