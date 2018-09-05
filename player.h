#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <map>
#include <vector>
#include "follower.h"
#include "followertype.h"
#include "occupationmapper.h"

using namespace std;

enum PlayerColor { red, blue, green, yellow, black, grey };

struct colorMap : public map<string, PlayerColor> {
	colorMap() {
		this->operator[]("red") = red;
		this->operator[]("blue") = blue;
		this->operator[]("green") = green;
		this->operator[]("yellow") = yellow;
		this->operator[]("black") = black;
		this->operator[]("grey") = grey;
	};
};

struct htmlColorMap : public map<PlayerColor, string> {
	htmlColorMap() {
		this->operator[](red) = "#ff0000";
		this->operator[](blue) = "#0000ff";
		this->operator[](green) = "#008000";
		this->operator[](yellow) = "#daa520";
		this->operator[](black) = "#000000";
		this->operator[](grey) = "#808080";
	};
};

class Follower;

class Player {
private:
	int points;
	int number;
	int barrels;
	int barley;
	int spools;
	PlayerColor color;
	vector<Follower*> followers;

public:
	Player(int num, string col);
	~Player();
	void addPoints(int p);
	void addMerchandise(int b, int g, int s);
	int getWine();
	int getGrain();
	int getCloth();
	int getPoints();
	int getPlayerNumber();
	PlayerColor getColor();
	void addFollower(FollowerType t, OccupationMapper *om, int amount);
	Follower* getFollower(FollowerType t);
	vector<Follower*> getAllFollowers();
	bool hasPlayableFollowers();
};
#endif
