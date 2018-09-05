#ifndef TILEIMAGE_H
#define TILEIMAGE_H
#include <vector>
#include <QSvgRenderer>
#include <QPainter>
#include <QImage>
#include <QTransform>
#include "tile.h"
#include "imagelayer.h"
#include "follower.h"
#include "piece.h"
#include "const.h"

using namespace std;

class Tile;
class Follower;
class Piece;

class TileImage {
private:
	vector<ImageLayer*> imageLayer;
	vector<Follower*> followers;
	int getRealRotation(int rot, int pos);
	vector<int> getRotationMatrix(int rot, int size);
	Tile *parent;
	Piece *piece;

public:
	TileImage(Tile *t);
	~TileImage();
	void addImageLayer(ImageLayer *l);
	void addFollower(Follower *f);
	void removeFollower(Follower *f);
	void removeFollowers();
	void setPiece(Piece *p);
	QImage* getImage(int size = IMG_DEFAULT_SIZE);
};
#endif
