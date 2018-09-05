#include "follower.h"

Follower::Follower(Player *p, FollowerType t, OccupationMapper *om) : parent(p), type(t), occupationMapper(om) {
	placement = NULL;
	playable = true;
	pigs = false;
	fairy = false;
	QFile *file;
	switch(t) {
	case normal:
	case big:
		file = new QFile("graphics/follower.svg"); // Deleted at and of function.
		break;
	case builder:
		file = new QFile("graphics/builder.svg");
		break;
	case pig:
		file = new QFile("graphics/pig.svg");
		break;
	}
	file->open(QIODevice::ReadOnly);
	QByteArray data = file->readAll();
	// All followers are red in the xml file - here we just do a string
	// substitution to get different colors. Yellow is handled differently
	// because it's actually encoded as goldenrod (#daa520). This is because
	// colors are also used for player text, and yellow text is unreadable.
	// Goldenrod followers not very visible, though, so special handling needed.
	if (parent->getColor() == yellow) {
		data.replace("#ff0000", "#ffff00");
	}
	else {
		data.replace("#ff0000", htmlColorMap()[parent->getColor()].c_str());
	}
	imageLayer = new ImageLayer(); // Deleted in destructor
	imageLayer->renderer = new QSvgRenderer(data); // Deleted in destructor
	imageLayer->scale = 0.2;
	occupationLayer.scale = 0.1;
	if (t == big) {
		imageLayer->scale = 0.3;
		occupationLayer.scale = 0.15;
	}
	delete file;
}

Follower::~Follower() {
	delete imageLayer->renderer;
	imageLayer->renderer = NULL;
	delete imageLayer;
	imageLayer = NULL;
}

FollowerType Follower::getType() {
	return type;
}

Player* Follower::getParent() {
	return parent;
}

Subtile* Follower::getPlacement() {
	return placement;
}

void Follower::setPigs(bool p) {
	pigs = p;
}

void Follower::setFairy(bool f) {
	fairy = f;
}

void Follower::setPlacement(Subtile *s) {
	placement = s;
	if (placement != NULL) {
		playable = false;
		translateSubtileToCoordinates(placement->getPosition(), &(imageLayer->x), &(imageLayer->y));
		if (type == builder || type == pig) {
			occupation = not_applicable;
		}
		else if (placement->getTerrainAddon() == monastery) {
			occupation = monk;
		}
		else {
			switch(placement->getTerrain()) {
			case city:
				occupation = knight;
				break;
			case road:
				occupation = robber;
				break;
			case field:
				occupation = farmer;
				break;
			default: // We should never get here.
				return;
			}
		}
		occupationLayer.renderer = occupationMapper->getRenderer(occupation);
		occupationLayer.rotation = imageLayer->rotation;
		occupationLayer.x = imageLayer->x + 20;
		occupationLayer.y = imageLayer->y + 20;
		if (type == big) {
			occupationLayer.x += 10;
		}
	}
	else {
		playable = true;
		occupation = not_applicable;
		pigs = false;
		fairy = false;
	}
}

bool Follower::isPlayable() {
	return playable;
}

bool Follower::hasPigs() {
	return pigs;
}

bool Follower::hasFairy() {
	return fairy;
}

Occupation Follower::getOccupation() {
	return occupation;
}

ImageLayer* Follower::getImage() {
	return imageLayer;
}

ImageLayer* Follower::getOccupationImage() {
	if (type == normal || type == big) {
		return &occupationLayer;
	}
	return NULL;
}

void Follower::translateSubtileToCoordinates(int subtile, int *x, int *y) {
	double scale = 1;
	if (type == big) {
		scale = 0.8;
	}
	switch(subtile) {
	case 0:
		*x = 40 * scale;
		*y = 0;
		return;
	case 1:
		*x = 160 * scale;
		*y = 0;
		return;
	case 2:
		*x = 300 * scale;
		*y = 0;
		return;
	case 3:
		*x = 330 * scale;
		*y = 30 * scale;
		return;
	case 4:
		*x = 330 * scale;
		*y = 160 * scale;
		return;
	case 5:
		*x = 330 * scale;
		*y = 300 * scale;
		return;
	case 6:
		*x = 300 * scale;
		*y = 320 * scale;
		return;
	case 7:
		*x = 160 * scale;
		*y = 320 * scale;
		return;
	case 8:
		*x = 40 * scale;
		*y = 320 * scale;
		return;
	case 9:
		*x = 0;
		*y = 300 * scale;
		return;
	case 10:
		*x = 0;
		*y = 160 * scale;
		return;
	case 11:
		*x = 0;
		*y = 30 * scale;
		return;
	case 12:
		*x = 160 * scale;
		*y = 100 * scale;
		return;
	case 13:
		*x = 220 * scale;
		*y = 160 * scale;
		return;
	case 14:
		*x = 160 * scale;
		*y = 220 * scale;
		return;
	case 15:
		*x = 110 * scale;
		*y = 160 * scale;
		return;
	default:
		*x = 0;
		*y = 0;
	}
}
