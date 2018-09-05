#include "player.h"

Player::Player(int num, string col) : number(num) {
	points = 0;
	color = colorMap()[col];
	barrels = 0;
	barley = 0;
	spools = 0;
}

Player::~Player() {
	for (int i = 0; i < followers.size(); ++i) {
		delete followers.at(i);
	}
}

void Player::addPoints(int p) {
	points += p;
}

void Player::addMerchandise(int b, int g, int s) {
	barrels += b;
	barley += g;
	spools += s;
}

int Player::getWine() {
	return barrels;
}

int Player::getGrain() {
	return barley;
}

int Player::getCloth() {
	return spools;
}

int Player::getPoints() {
	return points;
}

int Player::getPlayerNumber() {
	return number;
}

PlayerColor Player::getColor() {
	return color;
}

void Player::addFollower(FollowerType t, OccupationMapper *om, int amount = 1) {
	for (int i = 0; i < amount; ++i) {
		followers.push_back(new Follower(this, t, om)); // Deleted in destructor
	}
}

Follower* Player::getFollower(FollowerType t) {
	for (int i = 0; i < followers.size(); ++i) {
		if (followers.at(i)->getType() == t && followers.at(i)->isPlayable()) {
			return followers.at(i);
		}
	}
	return NULL;
}

vector<Follower*> Player::getAllFollowers() {
	return followers;
}

bool Player::hasPlayableFollowers() {
	for (int i = 0; i < followers.size(); ++i) {
		if (followers.at(i)->isPlayable()) {
			return true;
		}
	}
	return false;
}
