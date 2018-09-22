#ifndef DECK_H
#define DECK_H
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <cstdlib>
#include <QXmlStreamReader>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QSvgRenderer>
#include "tile.h"
#include "imagelayer.h"

using namespace std;

class Deck {
private:
	vector<Tile*> tiles;
	map<QString, QSvgRenderer*> baseImages;
	QString *baseDir;

public:
	Deck(QString *base);
	~Deck();
	void loadTiles(QString filename);
	void shuffleDeck();
	void shuffleDeck(int from);
	void shuffleDeck(int from, int to);
	void eraseTile(int num);
	void swapTiles(int num1, int num2);
	void mixIntoDeck(int num);
	int getSize();
	Tile* getTile(int position);
	QImage* getImage(QString filename);
};
#endif
