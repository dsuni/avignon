#include "piece.h"

Piece::Piece(QString filename, int x, int y, double scale) {
	imageLayer = new ImageLayer();
	imageLayer->renderer = new QSvgRenderer(filename);
	imageLayer->x = x;
	imageLayer->y = y;
	imageLayer->scale = scale;
	imageLayer->stationary = true;
	subtile = NULL;
}

Piece::~Piece() {
	delete imageLayer->renderer;
	delete imageLayer;
}

ImageLayer* Piece::getImage() {
	return imageLayer;
}

void Piece::setSubtile(Subtile *s) {
	subtile = s;
}

Subtile* Piece::getSubtile() {
	return subtile;
}

void Piece::setPosition(int x, int y) {
	imageLayer->x = x;
	imageLayer->y = y;	
}
