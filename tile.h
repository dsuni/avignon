#ifndef TILE_H
#define TILE_H
#include <vector>
#include <map>
#include <string>
#include <QSvgRenderer>
#include <QPixmap>
#include <QImage>
#include "subtile.h"
#include "tileimage.h"
#include "const.h"

using namespace std;

class TileImage;
class Subtile;

/* Diagram of a tile showing the subtiles' positions.
_________________
|\ 0 | 1   | 2 /|
| \  |     |  / |
|  \ |     | /  |
|11 \|     |/ 3 |
|---------------|
|    |\12 /|    |
|    | \ / |    |
|10  |15X13|  4 |
|    | / \ |    |
|    |/14 \|    |
|---------------|
|9  /|     |\ 5 |
|  / |     | \  |
| / 8| 7   |6 \ |
|/___|_____|___\|
*/

class Tile {
private:
	Subtile *subtiles[16];
	TileImage *image;
	int rotation;
	int posX, posY;
	bool dragon, fairy;
	static constexpr int opposite[16] = {8, 7, 6, 11, 10, 9, 2, 1, 0, 5, 4, 3, 14, 15, 12, 13};
	vector<Subtile*> distinctFeatures;
	map<Terrain, bool> presentTerrains;
	map<TerrainAddon, bool> presentTerrainAddons;
	void setupDistinctFeatures(Subtile *s, vector<Subtile*> *added);

 public:
	Tile();
	~Tile();
	void addSubtile(Subtile *s, int pos);
	void setDragon(bool d);
	void setFairy(bool f);
	int getTruePosition(int pos);
	int getFairyPosition(int pos);
	Subtile* getSubtile(int position, bool truePosition = true);
	TileImage* getImageContainer();
	QImage* getImage(int size = IMG_DEFAULT_SIZE);
	QPixmap* getPixmap(int size = IMG_DEFAULT_SIZE);
	int getRotation();
	int getX();
	int getY();
	bool hasDragon();
	bool hasFairy();
	void setPosition(int x, int y);
	void connectSubtiles(int s1, int s2);
	void rotateClockwise();
	void rotateCounterClockwise();
	bool fitsTogether(Tile *t, int pos);
	void placeTogether(Tile *t, int pos);
	void launchSetup();
	vector<Subtile*> getDistinctFeatures();
	bool hasTerrainAddon(TerrainAddon t);
	bool hasTerrain(Terrain t);
	vector<Subtile*> getTerrainAddon(TerrainAddon t);
	set<int> getRiverEdge(bool north, bool east, bool south, bool west);
	Follower* getMonk();
};
#endif
