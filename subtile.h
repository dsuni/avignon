#ifndef SUBTILE_H
#define SUBTILE_H
#include <vector>
#include <set>
#include <iostream>
#include "terrain.h"
#include "tile.h"
#include "follower.h"

using namespace std;

class Tile;
class Follower;

class Subtile {
private:
	Terrain terrain;
	TerrainAddon addon;
	vector<Subtile*> connections;
	Tile *parent;
	int position;
	Follower *follower;
	void addFeatureSubtile(int num, Terrain feature, set<Subtile*> *cities);
	void getFeatureOnTile(Subtile *s, set<Subtile*> *subtiles);

public:
	Subtile(Terrain t, TerrainAddon ta, Tile *p, int pos);
	void addConnection(Subtile *s);
	void setAddon(TerrainAddon ta);
	void setFollower(Follower *f);
	Follower* getFollower();
	Terrain getTerrain();
	TerrainAddon getTerrainAddon();
	vector<Subtile*> getConnections();
	set<Subtile*> getNeighboringFeatures(Terrain feature);
	Tile* getParent();
	int getPosition();
	bool isClosed();
	bool isOnSameFeatureAndTile(Subtile *s);
};
#endif
