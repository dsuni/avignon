#ifndef IMAGELAYER_H
#define IMAGELAYER_H
#include <QSvgRenderer>

struct ImageLayer {
ImageLayer() : renderer(NULL), rotation(0), x(0), y(0), stationary(false), scale(1) {}
	QSvgRenderer *renderer;
	int rotation;
	int x;
	int y;
	bool stationary;
	double scale;
};
#endif
