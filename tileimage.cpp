#include "tileimage.h"

TileImage::TileImage(Tile *t) : parent(t) {
	piece = NULL;
}

TileImage::~TileImage() {
	for (int i = 0; i < imageLayer.size(); ++i) {
		delete imageLayer.at(i);
	}
}

int TileImage::getRealRotation(int rot, int pos) {
	rot += imageLayer.at(pos)->rotation;
	if (rot >= 4) {
		rot -= 4;
	}
	return rot;
}

vector<int> TileImage::getRotationMatrix(int rot, int size) {
	vector<int> matrix;
	switch (rot) {
	case 0:
		matrix.push_back(1);
		matrix.push_back(0);
		matrix.push_back(0);
		matrix.push_back(1);
		matrix.push_back(0);
		matrix.push_back(0);
		break;
	case 1:
		matrix.push_back(0);
		matrix.push_back(1);
		matrix.push_back(-1);
		matrix.push_back(0);
		matrix.push_back(size);
		matrix.push_back(0);
		break;
	case 2:
		matrix.push_back(-1);
		matrix.push_back(0);
		matrix.push_back(0);
		matrix.push_back(-1);
		matrix.push_back(size);
		matrix.push_back(size);
		break;
	case 3:
		matrix.push_back(0);
		matrix.push_back(-1);
		matrix.push_back(1);
		matrix.push_back(0);
		matrix.push_back(0);
		matrix.push_back(size);
		break;
	}
	return matrix;
}

void TileImage::addImageLayer(ImageLayer *l) {
	imageLayer.push_back(l);
}

void TileImage::addFollower(Follower *f) {
	followers.push_back(f);
}

void TileImage::removeFollower(Follower *f) {
	for (int i = 0; i < followers.size(); ++i) {
		if (followers.at(i) == f) {
			followers.erase(followers.begin() + i);
			return;
		}
	}
}

void TileImage::removeFollowers() {
	while (!followers.empty()) {
		followers.pop_back();
	}
}

void TileImage::setPiece(Piece *p) {
	piece = p;
}

QImage* TileImage::getImage(int size) {
	// First create tile from layers
	double zoomScale = 1.0 * size / IMG_DEFAULT_SIZE;
	QImage *image = new QImage(size, size, IMG_FORMAT); // Deletion is up to receiver
	QPainter painter(image);
	for (int i = 0; i < imageLayer.size(); ++i) {
		int realRot = getRealRotation(parent->getRotation(), i);
		vector<int> matrix = getRotationMatrix(realRot, size);
		QTransform trans(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
		trans.scale(imageLayer.at(i)->scale, imageLayer.at(i)->scale);
		trans.translate(imageLayer.at(i)->x * zoomScale / imageLayer.at(i)->scale, imageLayer.at(i)->y * zoomScale / imageLayer.at(i)->scale);
		if (!imageLayer.at(i)->stationary) {
			painter.setTransform(trans);
		}
		imageLayer.at(i)->renderer->render(&painter);
	}
	// Then add followers on top
	for (int i = 0; i < followers.size(); ++i) {
		int realRot = getRealRotation(parent->getRotation(), i);
		vector<int> matrix = getRotationMatrix(realRot, size);
		QTransform trans(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
		ImageLayer *il = followers.at(i)->getImage();
		trans.scale(il->scale, il->scale);
		trans.translate(il->x * zoomScale / il->scale, il->y * zoomScale / il->scale);
		painter.setTransform(trans);
		il->renderer->render(&painter);
		QTransform trans2(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
		il = followers.at(i)->getOccupationImage();
		if (il == NULL) {
			continue;
		}
		trans2.scale(il->scale, il->scale);
		trans2.translate(il->x * zoomScale / il->scale, il->y * zoomScale / il->scale);
		painter.setTransform(trans2);
		il->renderer->render(&painter);
	}
	// Finally add piece, if any
	if (piece != NULL) {
		vector<int> matrix = getRotationMatrix(0, size);
		QTransform trans(matrix[0], matrix[1], matrix[2], matrix[3], matrix[4], matrix[5]);
		ImageLayer *il = piece->getImage();
		trans.scale(il->scale, il->scale);
		trans.translate(il->x * zoomScale / il->scale, il->y * zoomScale / il->scale);
		painter.setTransform(trans);
		il->renderer->render(&painter);
	}
	return image;
}
