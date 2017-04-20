#include "Map.h"

Map::Map(string filePath) {

	loadFromFile(filePath);
}

void Map::loadFromFile(string filePath) {

	char c;
	char str[2048];
	ifstream is(str);

	File.open(filePath); // Open file

	if (!File) {		// If the file does not exist
		cout << "The file does not exist!" << endl;
		exit(1);
	}

	cin.get(str, 2048);
	

	while (is.get(c))
		cout << c;

	is.close();

}