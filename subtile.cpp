#include "subtile.h"

Subtile::Subtile(Terrain t, TerrainAddon ta, Tile *p, int pos) : terrain(t), addon(ta), parent(p), position(pos) {
	follower = NULL;
}

void Subtile::addConnection(Subtile *s) {
	// Verifying terrain types helps catch mistakes in the XML files
	if (terrain != s->getTerrain()) {
		cerr << "Mismatched terrain types " << terrain << " and " << s->getTerrain() << ". Check your XML files." << endl;
		for (int i = 0; i < 16; ++i) {
			cerr << "\tSubtile " << i << ": " << parent->getSubtile(i, false)->getTerrain() << endl;
		}
		return;
	}
	// Monasteries should not be mixed with other tiles
	if ((addon == monastery && s->getTerrainAddon() != monastery) || (addon != monastery && s->getTerrainAddon() == monastery)) {
		return;
	}
	connections.push_back(s);
}

void Subtile::setAddon(TerrainAddon ta) {
	addon = ta;
}

void Subtile::setFollower(Follower *f) {
	follower = f;
}

Follower* Subtile::getFollower() {
	return follower;
}

Terrain Subtile::getTerrain() {
	return terrain;
}

TerrainAddon Subtile::getTerrainAddon() {
	return addon;
}

vector<Subtile*> Subtile::getConnections() {
	return connections;
}

Tile* Subtile::getParent() {
	return parent;
}

int Subtile::getPosition() {
	return position;
}

// A subtile is closed when it's either in the centre of the tile
// (positions 12, 13, 14 & 15) or on the edge, but connected to another tile.
bool Subtile::isClosed() {
	if (position >= 12) {
		return true;
	}
	for (int i = 0; i < connections.size(); ++i) {
		if (parent != connections.at(i)->getParent()) {
			return true;
		}
	}
	return false;
}

set<Subtile*> Subtile::getNeighboringFeatures(Terrain feature) {
	set<Subtile*> subtiles;
	switch(position) {
	case 0:
		addFeatureSubtile(1, feature, &subtiles);
		addFeatureSubtile(11, feature, &subtiles);
		break;
	case 1:
		addFeatureSubtile(0, feature, &subtiles);
		addFeatureSubtile(2, feature, &subtiles);
		addFeatureSubtile(12, feature, &subtiles);
		break;
	case 2:
		addFeatureSubtile(1, feature, &subtiles);
		addFeatureSubtile(3, feature, &subtiles);
		break;
	case 3:
		addFeatureSubtile(2, feature, &subtiles);
		addFeatureSubtile(4, feature, &subtiles);
		break;
	case 4:
		addFeatureSubtile(3, feature, &subtiles);
		addFeatureSubtile(5, feature, &subtiles);
		addFeatureSubtile(13, feature, &subtiles);
		break;
	case 5:
		addFeatureSubtile(4, feature, &subtiles);
		addFeatureSubtile(6, feature, &subtiles);
		break;
	case 6:
		addFeatureSubtile(5, feature, &subtiles);
		addFeatureSubtile(7, feature, &subtiles);
		break;
	case 7:
		addFeatureSubtile(6, feature, &subtiles);
		addFeatureSubtile(8, feature, &subtiles);
		addFeatureSubtile(14, feature, &subtiles);
		break;
	case 8:
		addFeatureSubtile(7, feature, &subtiles);
		addFeatureSubtile(9, feature, &subtiles);
		break;
	case 9:
		addFeatureSubtile(8, feature, &subtiles);
		addFeatureSubtile(10, feature, &subtiles);
		break;
	case 10:
		addFeatureSubtile(9, feature, &subtiles);
		addFeatureSubtile(11, feature, &subtiles);
		addFeatureSubtile(15, feature, &subtiles);
		break;
	case 11:
		addFeatureSubtile(0, feature, &subtiles);
		addFeatureSubtile(10, feature, &subtiles);
		break;
	case 12:
		addFeatureSubtile(1, feature, &subtiles);
		addFeatureSubtile(13, feature, &subtiles);
		addFeatureSubtile(15, feature, &subtiles);
		break;
	case 13:
		addFeatureSubtile(4, feature, &subtiles);
		addFeatureSubtile(12, feature, &subtiles);
		addFeatureSubtile(14, feature, &subtiles);
		break;
	case 14:
		addFeatureSubtile(7, feature, &subtiles);
		addFeatureSubtile(13, feature, &subtiles);
		addFeatureSubtile(15, feature, &subtiles);
		break;
	case 15:
		addFeatureSubtile(10, feature, &subtiles);
		addFeatureSubtile(12, feature, &subtiles);
		addFeatureSubtile(14, feature, &subtiles);
		break;
	default:
		break;
	}
	return subtiles;
}

void Subtile::addFeatureSubtile(int num, Terrain feature, set<Subtile*> *subtiles) {
	Subtile *s = parent->getSubtile(num, false);
	if (s->getTerrain() == feature) {
		subtiles->insert(s);
	}
}

void Subtile::getFeatureOnTile(Subtile *s, set<Subtile*> *subtiles) {
	subtiles->insert(s);
	vector<Subtile*> conn = s->getConnections();
	for (int i = 0; i < conn.size(); ++i) {
		if (subtiles->find(conn.at(i)) != subtiles->end()) {
			continue;
		}
		if (s->getParent() != conn.at(i)->getParent()) {
			continue;
		}
		getFeatureOnTile(conn.at(i), subtiles);
	}
}

bool Subtile::isOnSameFeatureAndTile(Subtile *s) {
	if (s == NULL) {
		return false;
	}
	set<Subtile*> subtiles;
	getFeatureOnTile(s, &subtiles);
	for (set<Subtile*>::iterator it = subtiles.begin(); it != subtiles.end(); ++it) {
		Subtile *temp = *it;
		if (this == temp) {
			return true;
		}
	}
	return false;
}
