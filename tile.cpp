#include "tile.h"

constexpr int Tile::opposite[16];

Tile::Tile() {
	image = new TileImage(this); // Deleted in destructor
	rotation = 0;
	posX = -1;
	posY = -1;
	dragon = false;
	fairy = false;
}

Tile::~Tile() {
	for (int i = 0; i < 16; ++i) {
		delete subtiles[i];
	}
	delete image;
}

int Tile::getTruePosition(int pos) {
	// Tile center section
	if (pos >= 12) {
		int temp = pos - rotation;
		if (temp < 12) {
			temp += 4;
		}
		return temp;
	}
	// Tile outer section
	int temp = pos - rotation * 3;
	if (temp < 0) {
		temp += 12;
	}
	return temp;
}

int Tile::getFairyPosition(int pos) {
	if (pos >= 12) {
		int temp = pos + rotation;
		if (temp > 15) {
			temp -= 4;
		}
		return temp;
	}
	int temp = pos + rotation * 3;
	if (temp > 11) {
		temp -= 12;
	}
	return temp;
}

void Tile::addSubtile(Subtile *s, int pos) {
	subtiles[pos] = s;
}

void Tile::setDragon(bool d) {
	dragon = d;
	// Adding dragon to tile will automatically kick all followers off of it
	if (dragon) {
		for (int i = 0; i < 16; ++i) {
			Follower *f = subtiles[i]->getFollower();
			if (f != NULL) {
				f->setPlacement(NULL);
				subtiles[i]->setFollower(NULL);
			}
		}
	}
	// While removing it will automatically clear the piece from the image
	else {
		image->setPiece(NULL);
	}
}

void Tile::setFairy(bool f) {
	fairy = f;
	if (!fairy) {
		image->setPiece(NULL);
	}
}

Subtile* Tile::getSubtile(int position, bool truePosition) {
	if (truePosition) {
		return subtiles[getTruePosition(position)];
	}
	return subtiles[position];
}

TileImage* Tile::getImageContainer() {
	return image;
}

QImage* Tile::getImage(int size) {
	return image->getImage(size);
}

QPixmap* Tile::getPixmap(int size) {
	QImage *img = image->getImage(size);
	QPixmap *pxm = new QPixmap(QPixmap::fromImage(*img)); // Deleted by caller
	delete img;
	return pxm;
}

int Tile::getRotation() {
	return rotation;
}

int Tile::getX() {
	return posX;
}

int Tile::getY() {
	return posY;
}

bool Tile::hasDragon() {
	return dragon;
}

bool Tile::hasFairy() {
	return fairy;
}

void Tile::setPosition(int x, int y) {
	posX = x;
	posY = y;
}

void Tile::connectSubtiles(int s1, int s2) {
	subtiles[s1]->addConnection(subtiles[s2]);
	subtiles[s2]->addConnection(subtiles[s1]);
}

void Tile::rotateClockwise() {
	++rotation;
	if (rotation == 4) {
		rotation = 0;
	}
}

void Tile::rotateCounterClockwise() {
	--rotation;
	if (rotation == -1) {
		rotation = 3;
	}
}

// 'pos' variable indicates on which side to apply the other tile.
// 0 == north, 1 == east, 2 == south and 3 == west.
bool Tile::fitsTogether(Tile *t, int pos) {
	// No other tile, means no restrictions.
	if (t == NULL) {
		return true;
	}
	for (int i = pos * 3; i < pos * 3 + 3; ++i) {
		if (getSubtile(i)->getTerrain() != t->getSubtile(opposite[i])->getTerrain()) {
			return false;
		}
	}
	return true;
}

void Tile::placeTogether(Tile *t, int pos) {
	if (t == NULL) {
		return;
	}
	for (int i = pos * 3; i < pos * 3 + 3; ++i) {
		getSubtile(i)->addConnection(t->getSubtile(opposite[i]));
		t->getSubtile(opposite[i])->addConnection(getSubtile(i));
	}
}

// Get a vector containing exactly one subtile from each distinct
// feature on the tile. (This eases some processing later on.)
// This function must be called before any tiles have been placed.
void Tile::setupDistinctFeatures(Subtile *s, vector<Subtile*> *added) {
	if (find(added->begin(), added->end(), s) != added->end()) {
		return;
	}
	if (s->getTerrainAddon() == intersection) {
		added->push_back(s);
		return;
	}
	added->push_back(s);
	vector<Subtile*> subs = s->getConnections();
	for (int i = 0; i < subs.size(); ++i) {
		setupDistinctFeatures(subs.at(i), added);
	}
}

void Tile::launchSetup() {
	// Set up distinct features
	int mem;
	vector<Subtile*> added;
	for (int i = 0; i < 16; ++i) {
		if (find(added.begin(), added.end(), subtiles[i]) == added.end()) {
			mem = added.size();
			setupDistinctFeatures(subtiles[i], &added);
			distinctFeatures.push_back(added.at(mem));
		}
	}
	// Set up present terrain
	for (int i = terrainFirst; i <= terrainLast; ++i) {
		Terrain t = (Terrain)i;
		bool found = false;
		for (int j = 0; j < 16; ++j) {
			if (t == subtiles[j]->getTerrain()) {
				found = true;
				break;
			}
		}
		presentTerrains[t] = found;
	}
	// Set up present terrain addons
	for (int i = terrainAddonFirst; i <= terrainAddonLast; ++i) {
		TerrainAddon t = (TerrainAddon)i;
		bool found = false;
		for (int j = 0; j < 16; ++j) {
			if (t == subtiles[j]->getTerrainAddon()) {
				found = true;
				break;
			}
		}
		presentTerrainAddons[t] = found;
	}
}

vector<Subtile*> Tile::getDistinctFeatures() {
	return distinctFeatures;
}

bool Tile::hasTerrainAddon(TerrainAddon t) {
	return presentTerrainAddons[t];
}

bool Tile::hasTerrain(Terrain t) {
	return presentTerrains[t];
}

vector<Subtile*> Tile::getTerrainAddon(TerrainAddon t) {
	vector<Subtile*> s;
	for (int i = 0; i < 16; ++i) {
		if (subtiles[i]->getTerrainAddon() == t) {
			s.push_back(subtiles[i]);
		}
	}
	return s;
}

// Returns a set of integers (0,1,2,3) <=> (N,E,S,W) describing which edges
// of the tile has a river. Only the edges where the corresponding boolean is
// true will be included. E.g. a river fork with edges to N, E & W, called
// with (false, true, true, true) will return a set containing 1 and 3.
set<int> Tile::getRiverEdge(bool north, bool east, bool south, bool west) {
	set<int> edge;
	for (int i = 1; i <= 10; i += 3) {
		if (getSubtile(i)->getTerrain() == river) {
			if ((north && i == 1) || (east && i == 4) || (south && i == 7) || (west && i == 10)) {
				edge.insert((i - 1) / 3);
			}
		}
	}
	return edge;
}

Follower* Tile::getMonk() {
	for (int i = 0; i < 16; ++i) {
		if (subtiles[i]->getTerrainAddon() == monastery) {
			Follower *f = subtiles[i]->getFollower();
			if (f != NULL) {
				return f;
			}
		}
	}
	return NULL;
}
