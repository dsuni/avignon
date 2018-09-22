#ifndef FOLLOWER_H
#define FOLLOWER_H
#include <QFile>
#include <QByteArray>
#include <QApplication>
#include "player.h"
#include "subtile.h"
#include "followertype.h"
#include "imagelayer.h"
#include "occupationmapper.h"

using namespace std;

class Player;
class Subtile;

class Follower {
private:
	Player *parent;
	FollowerType type;
	Subtile *placement;
	Occupation occupation;
	OccupationMapper *occupationMapper;
	bool playable;
	bool pigs;
	bool fairy;
	ImageLayer *imageLayer;
	ImageLayer occupationLayer;

public:
	Follower(Player *p, FollowerType t, OccupationMapper *om);
	~Follower();
	FollowerType getType();
	Occupation getOccupation();
	Player* getParent();
	Subtile* getPlacement();
	void setPlacement(Subtile *s);
	void setPigs(bool p);
	void setFairy(bool f);
	void translateSubtileToCoordinates(int subtile, int *x, int *y);
	bool isPlayable();
	bool hasPigs();
	bool hasFairy();
	ImageLayer* getImage();
	ImageLayer* getOccupationImage();
};
#endif
