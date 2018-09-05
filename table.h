#ifndef TABLE_H
#define TABLE_H
#include <cstddef>
#include <vector>
#include <set>
#include <QPixmap>
#include <QDebug>
#include "tile.h"
#include "tablelabel.h"
#include "const.h"

using namespace std;

class Table {
private:
	int width;
	int height;
	int barrels;
	int barley;
	int spools;
	int dragonPos;
	int fairyPos;
	bool innsAndCathedrals;
	bool princessAndDragon;
	bool fairyScoredFor;
	vector<Tile*> tiles;
	vector<TableLabel*> labels;
	vector<set<Subtile*>> completedCities;
	vector<vector<bool>> farmerCityCoverage; // Keeps track of which players have scored for which cities
	vector<vector<bool>> pigCityCoverage; // Keeps track of which players have scored for pigs for which cities
	int hasIntersection(Subtile *s, Subtile *sender, set<Subtile*> *visited);
	void getEntireFeature(Subtile *s, set<Subtile*> *visited);
	int getMaxValue(vector<int> val);

public:
	Table(int w, int h);
	~Table();
	void addTile(Tile *t, int x, int y);
	Tile* getTile(int x, int y);
	void setExpansions(bool inc, bool pnd);
	int getWidth();
	int getHeight();
	void setDragon(int x, int y);
	void setFairy(int x, int y);
	void getDragon(int *x, int *y);
	void getFairy(int *x, int *y);
	void getMerchandise(int *b, int *g, int *c);
	vector<Tile*> getNeighbors(int x, int y);
	vector<Tile*> getMonasteryNeighbors(Tile *t);
	vector<Tile*> getTiles();
	vector<TableLabel*> getLabels();
	TableLabel* getLabel(int x, int y);
	bool tileFits(Tile *t, int x, int y);
	bool hasNeighbors(int x, int y);
	bool isRoadComplete(Subtile *s);
	bool isCityComplete(Subtile *s);
	bool isMonasteryComplete(Tile *t);
	bool doesFeatureContainFollowers(Subtile *s, bool pointGiversOnly);
	bool doesFeatureContainPigs(Subtile *s, Player *p);
	set<Follower*> getAllFollowersInFeature(Subtile *s);
	int getFeatureScore(Subtile *s, int rules);
	int getFeatureScore(Subtile *s, int rules, int player, vector<bool> pigsOnField, bool gameOver);
	int getTilePosition(Tile *t);
	void setPixmap(int x, int y, QPixmap *pm);
	vector<int> scoreFirstEditionFarms();
};
#endif
