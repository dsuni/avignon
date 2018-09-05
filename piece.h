#ifndef PIECE_H
#define PIECE_H
#include <QString>
#include <QSvgRenderer>
#include "imagelayer.h"
#include "subtile.h"

using namespace std;

class Subtile;

class Piece {
private:
	ImageLayer *imageLayer;
	Subtile *subtile;

public:
	Piece(QString filename, int x, int y, double scale);
	~Piece();
	ImageLayer* getImage();
	void setSubtile(Subtile *s);
	Subtile* getSubtile();
	void setPosition(int x, int y);
};
#endif
