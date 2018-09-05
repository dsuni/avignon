#include "table.h"

Table::Table(int w, int h) : width(w), height(h) {
	tiles.assign(w * h, NULL);
	for (int i = 0; i < w * h; ++i) {
		labels.push_back(new TableLabel()); // Deleted in destructor
		labels[i]->setMargin(0);
		labels[i]->setPosition(i % w, i / w);
	}
	// One of each type of vector for each player. (If less than 6 players
	// those vectors will just remain unused).
	for (int i = 0; i < 6; ++i) {
		vector<bool> v;
		vector<bool> w;
		farmerCityCoverage.push_back(v);
		pigCityCoverage.push_back(w);
	}
	barrels = 0;
	barley = 0;
	spools = 0;
	dragonPos = -1;
	fairyPos = -1;
	innsAndCathedrals = false;
	princessAndDragon = false;
	fairyScoredFor = false;
}

Table::~Table() {
	for (int i = 0; i < width * height; ++i) {
		delete labels.at(i);
		labels.at(i) = NULL;
	}
}

void Table::addTile(Tile *t, int x, int y) {
	tiles.at(y * width + x) = t;
	vector<Tile*> neighbors = getNeighbors(x, y);
	for (int i = 0; i < 4; ++i) {
		t->placeTogether(neighbors.at(i), i);
	}
}

Tile* Table::getTile(int x, int y) {
	return tiles.at(y * width + x);
}

void Table::setExpansions(bool inc, bool pnd) {
	innsAndCathedrals = inc;
	princessAndDragon = pnd;
}

int Table::getWidth() {
	return width;
}

int Table::getHeight() {
	return height;
}

void Table::setDragon(int x, int y) {
	if (dragonPos >= 0) {
		tiles.at(dragonPos)->setDragon(false);
	}
	dragonPos = y * width + x;
	tiles.at(dragonPos)->setDragon(true);
}

void Table::setFairy(int x, int y) {
	if (fairyPos >= 0) {
		tiles.at(fairyPos)->setFairy(false);
	}
	fairyPos = y * width + x;
	tiles.at(fairyPos)->setFairy(true);
}

void Table::getDragon(int *x, int *y) {
	if (dragonPos < 0) {
		*x = -1;
		*y = -1;
		return;
	}
	*x = dragonPos % width;
	*y = dragonPos / width;
}

void Table::getFairy(int *x, int *y) {
	if (fairyPos < 0) {
		*x = -1;
		*y = -1;
		return;
	}
	*x = fairyPos % width;
	*y = fairyPos / width;
}

void Table::getMerchandise(int *b, int *g, int *c) {
	*b = barrels;
	*g = barley;
	*c = spools;
}

vector<Tile*> Table::getTiles() {
	return tiles;
}

vector<TableLabel*> Table::getLabels() {
	return labels;
}

// Returns tiles to the north, east, south and west of the tile - in that order.
vector<Tile*> Table::getNeighbors(int x, int y) {
	vector<Tile*> neighbors;
	if (y == 0) {
		neighbors.push_back(NULL);
	}
	else {
		neighbors.push_back(tiles.at((y - 1) * width + x));
	}
	if (x == width - 1) {
		neighbors.push_back(NULL);
	}
	else {
		neighbors.push_back(tiles.at(y * width + x + 1));
	}
	if (y == height - 1) {
		neighbors.push_back(NULL);
	}
	else {
		neighbors.push_back(tiles.at((y + 1) * width + x));
	}
	if (x == 0) {
		neighbors.push_back(NULL);
	}
	else {
		neighbors.push_back(tiles.at(y * width + x - 1));
	}
	return neighbors;
}

// Returns the cluster of 9 tiles relevant to a monastery
vector<Tile*> Table::getMonasteryNeighbors(Tile *t) {
	vector<Tile*> neighbors;
	int pos = getTilePosition(t);
	int x = pos % width;
	int y = pos / width;
	for (int i = x - 1; i <= x + 1; ++i) {
		for (int j = y - 1; j <= y + 1; ++j) {
			if (i < 0 || j < 0 || i == width || j == height) {
				neighbors.push_back(NULL);
			}
			else {
				neighbors.push_back(tiles.at(j * width + i));
			}
		}
	}
	return neighbors;
}

TableLabel* Table::getLabel(int x, int y) {
	return labels.at(y * width + x);
}

bool Table::tileFits(Tile *t, int x, int y) {
	vector<Tile*> neighbors = getNeighbors(x, y);
	for (int i = 0; i < 4; ++i) {
		if (!t->fitsTogether(neighbors.at(i), i)) {
			return false;
		}
	}
	return true;
}

bool Table::hasNeighbors(int x, int y) {
	vector<Tile*> neighbors = getNeighbors(x, y);
	for (int i = 0; i < 4; ++i) {
		if (neighbors.at(i) != NULL) {
			return true;
		}
	}
	return false;
}

bool Table::isRoadComplete(Subtile *s) {
	int intersections = 0;
	if (s->getTerrainAddon() == intersection) {
		++intersections;
	}
	set<Subtile*> visited;
	vector<Subtile*> conn = s->getConnections();
	for (int i = 0; i < conn.size(); ++i) {
		intersections += hasIntersection(conn.at(i), s, &visited);
	}
	// Intersections can actually exceed 2 in case the road loops.
	if (intersections >= 2) {
		return true;
	}
	return false;
}

int Table::hasIntersection(Subtile *s, Subtile *sender, set<Subtile*> *visited) {
	int size = visited->size();
	visited->insert(s);
	// If size is the same, s was already a member of the set -> road makes a loop -> road is complete
	if (size == visited->size()) {
		return 2;
	}
	if (s->getTerrainAddon() == intersection) {
		return 1;
	}
	vector<Subtile*> conn = s->getConnections();
	for (int i = 0; i < conn.size(); ++i) {
		if (conn.at(i) == sender) {
			continue;
		}
		return hasIntersection(conn.at(i), s, visited);
	}
	return 0;
}

bool Table::isCityComplete(Subtile *s) {
	if (s->getTerrain() != city) {
		return false;
	}
	// Check if the subtile is part of an already known completed city
	if (!completedCities.empty()) {
		// If not using Princess & dragon rules, we only need to check the last
		// completed city (P&D has portals that allow arbitrary placement -> check all)
		int from = 0;
		if (!princessAndDragon) {
			from = completedCities.size() - 1;
		}
		for (int i = from; i < completedCities.size(); ++i) {
			for (set<Subtile*>::iterator it = completedCities.at(i).begin(); it != completedCities.at(i).end(); ++it) {
				Subtile *temp = *it;
				if (temp == s) {
					return true;
				}
			}
		}
	}
	// If not, check if it's a newly completed city.
	set<Subtile*> visited;
	getEntireFeature(s, &visited);
	for (set<Subtile*>::iterator it=visited.begin(); it!=visited.end(); ++it) {
		Subtile *temp = *it;
		if (!temp->isClosed()) {
			return false;
		}
	}
	completedCities.push_back(visited);
	return true;
}

bool Table::isMonasteryComplete(Tile *t) {
	vector<Tile*> neighbors = getMonasteryNeighbors(t);
	for (int i = 0; i < neighbors.size(); ++i) {
		if (neighbors.at(i) == NULL) {
			return false;
		}
	}
	return true;
}

bool Table::doesFeatureContainFollowers(Subtile *s, bool pointGiversOnly) {
	set<Follower*> followers = getAllFollowersInFeature(s);
	if (!pointGiversOnly) {
		return !followers.empty();
	}
	for (set<Follower*>::iterator it = followers.begin(); it != followers.end(); ++it) {
		Follower *f = *it;
		if (f->getType() == normal || f->getType() == big) {
			return true;
		}
	}
	return false;
}

// Returns true if feature either contains the pig herd (from river II), or the
// relevant player has added a pig follower to the field, otherwise false.
bool Table::doesFeatureContainPigs(Subtile *s, Player *p) {
	set<Subtile*> feature;
	getEntireFeature(s, &feature);
	for (set<Subtile*>::iterator it = feature.begin(); it != feature.end(); ++it) {
		Subtile *temp = *it;
		if (temp->getTerrainAddon() == pigs) {
			return true;
		}
		if (temp->getFollower() != NULL && temp->getFollower()->getType() == pig && temp->getFollower()->getParent() == p) {
			return true;
		}
	}
	return false;
}

void Table::getEntireFeature(Subtile *s, set<Subtile*> *visited) {
	visited->insert(s);
	vector<Subtile*> conn = s->getConnections();
	for (int i = 0; i < conn.size(); ++i) {
		if (visited->find(conn.at(i)) == visited->end()) {
			getEntireFeature(conn.at(i), visited);
		}
	}
}

set<Follower*> Table::getAllFollowersInFeature(Subtile *s) {
	set<Follower*> followers;
	set<Subtile*> visited;
	getEntireFeature(s, &visited);
	for (set<Subtile*>::iterator it=visited.begin(); it!=visited.end(); ++it) {
		Subtile *temp = *it;
		if (temp->getFollower() != NULL) {
			followers.insert(temp->getFollower());
		}
	}
	return followers;
}

int Table::getFeatureScore(Subtile *s, int rules) {
	vector<bool> fake;
	return getFeatureScore(s, rules, 0, fake, false);
}

int Table::getFeatureScore(Subtile *s, int rules, int player, vector<bool> pigsOnField, bool gameOver) {
	barrels = 0;
	barley = 0;
	spools = 0;
	set<Tile*> tiles;
	int shields = 0;
	int score = 0;
	int pointsPerCityTile = 2;
	bool innRoad = false;
	bool cathedralCity = false;
	set<Subtile*> visited;
	getEntireFeature(s, &visited);
	for (set<Subtile*>::iterator it=visited.begin(); it!=visited.end(); ++it) {
		Subtile *temp = *it;
		tiles.insert(temp->getParent());
		switch(temp->getTerrainAddon()) {
		case shield:
			++shields;
			break;
		case inn:
			innRoad = innsAndCathedrals; // true iff playing with I&C ruleset
			break;
		case cathedral:
			cathedralCity = innsAndCathedrals;
			break;
		case pigs: {
			for (int i = 0; i < pigsOnField.size(); ++i) {
				pigsOnField.at(i) = true;
			}
			break;
		}
		case wine:
			++barrels;
			break;
		case grain:
			++barley;
			break;
		case cloth:
			++spools;
			break;
		case monastery: {
			vector<Tile*> temp = getMonasteryNeighbors(s->getParent());
			for (int i = 0; i < temp.size(); ++i) {
				if (temp.at(i) != NULL) {
					++score;
				}
			}
			return score;
		}
		default:
			break;
		}
	}
	switch (s->getTerrain()) {
	case city:
		// With 1st & 2nd ed. rules, size 2 cities only score 1 point/tile
		if (gameOver || (rules <= 2 && tiles.size() <= 2)) {
			// Unfinished cities with cathedral gives no points
			if (cathedralCity) {
				return 0;
			}
			return tiles.size();
		}
		if (cathedralCity) {
			++pointsPerCityTile;
		}
		return pointsPerCityTile * tiles.size() + pointsPerCityTile * shields;
	case road:
		if (innRoad) {
			// We should get here only when feature is complete or game is over.
			if (!gameOver) {
				return 2 * tiles.size();
			}
			return 0;
		}
		return tiles.size();
	case field:
		// 1st edition rules work very differently, and that calculation is done
		// elsewhere (specifically scoreFirstEditionFarms().) This means that
		// by the time we get here, those points have already been added, so we
		// just return 0, in order to not add any more.
		if (rules == 1) {
			return 0;
		}
		set<Subtile*> neighboringCities;
		// If it's the first field to be counted we need to set up the city coverage bookkeeping
		if (farmerCityCoverage.at(player).empty()) {
			farmerCityCoverage.at(player).assign(completedCities.size(), false);
			pigCityCoverage.at(player).assign(completedCities.size(), false);
		}
		// According to 3rd edition rules a city can be scored for multiple times
		// if the farmers are on different fields -> We need to reset the coverage.
		else if (rules == 3) {
			for (int i = 0; i < farmerCityCoverage.at(player).size(); ++i) {
				farmerCityCoverage.at(player).at(i) = false;
				pigCityCoverage.at(player).at(i) = false;
			}
		}
		// Gather all city subtiles that abutt the field in a set
		for (set<Subtile*>::iterator it=visited.begin(); it!=visited.end(); ++it) {
			Subtile *temp = *it;
			set<Subtile*> local = temp->getNeighboringFeatures(city);
			neighboringCities.insert(local.begin(), local.end());
		}
		// Parse the tiles...
		for (set<Subtile*>::iterator it=neighboringCities.begin(); it!=neighboringCities.end(); ++it) {
			Subtile *temp = *it;
			// ...and check if they're part of a completed city.
			for (int i = 0; i < completedCities.size(); ++i) {
				// We're already done if we've already scored for this city AND either can't
				// score for pigs OR already have scored for pigs for this city.
				if (farmerCityCoverage.at(player).at(i) && (!pigsOnField.at(player) || pigCityCoverage.at(player).at(i)) ) {
						continue;
					}
				if (completedCities.at(i).find(temp) != completedCities.at(i).end()) {
					if (!farmerCityCoverage.at(player).at(i)) {
						score += 3;
						farmerCityCoverage.at(player).at(i) = true;
						if (rules == 3 && pigsOnField.at(player) && !pigCityCoverage.at(player).at(i)) {
							++score;
							pigCityCoverage.at(player).at(i) = true;
						}
					}
					if (rules == 2 && pigsOnField.at(player) && !pigCityCoverage.at(player).at(i)) {
						++score;
						pigCityCoverage.at(player).at(i) = true;
					}
				}
			}
		}
		return score;
	}
	return 0;
}

int Table::getTilePosition(Tile *t) {
	for (int i = 0; i < tiles.size(); ++i) {
		if (tiles.at(i) == t) {
			return i;
		}
	}
	return -1;
}

void Table::setPixmap(int x, int y, QPixmap *pm) {
	getLabel(x, y)->setPixmap(*pm);
}

// 1st edition scoring means: For each completed city, find all farmers who
// provide for that city. The player(s) with the most farmers recieve 4 pts
// for that city (5 if at least one of that player's farmers has pigs).
vector<int> Table::scoreFirstEditionFarms() {
	vector<int> farmScore;
	farmScore.assign(6, 0);
	// Parse through all completed cities.
	for (int i = 0; i < completedCities.size(); ++i) {
		Subtile *s1 = *completedCities.at(i).begin();
		set<Subtile*> neighboringFields;
		set<Follower*> neighboringFollowers;
		vector<int> playerWeight;
		playerWeight.assign(6, 0);
		vector<bool> pigs;
		pigs.assign(6, false);
		// For each city, gather all field tiles that abutt that city...
		for (set<Subtile*>::iterator it = completedCities.at(i).begin(); it != completedCities.at(i).end(); ++it) {
			Subtile *temp = *it;
			set<Subtile*> local = temp->getNeighboringFeatures(field);
			neighboringFields.insert(local.begin(), local.end());		
		}
		// ...parse through those tiles adding all the followers into a set.
		for (set<Subtile*>::iterator it = neighboringFields.begin(); it != neighboringFields.end(); ++it) {
			Subtile *temp = *it;
			set<Follower*> local = getAllFollowersInFeature(temp);
			neighboringFollowers.insert(local.begin(), local.end());
		}
		// Finally parse through the followers, accounting for their weight.
		vector<bool> playerHasFairy;
		playerHasFairy.assign(6, false);
		for (set<Follower*>::iterator it = neighboringFollowers.begin(); it != neighboringFollowers.end(); ++it) {
			Follower *temp = *it;
			int playerNum = temp->getParent()->getPlayerNumber();
			switch (temp->getType()) {
			case big:
				++playerWeight.at(playerNum);
				// no break is intentional
			case normal:
				++playerWeight.at(playerNum);
				if (temp->hasPigs()) {
					pigs.at(playerNum) = true;
				}
				if (temp->hasFairy()) {
					playerHasFairy.at(playerNum) = true;
				}
				break;
			default:
				break;
			}
		}
		int max = getMaxValue(playerWeight);
		// If nobody has farmers, nobody gets points.
		if (max == 0) {
			continue;
		}
		for (int i = 0; i < playerWeight.size(); ++i) {
			if (playerWeight.at(i) == max) {
				farmScore.at(i) += 4;
				if (pigs.at(i)) {
					++farmScore.at(i);
				}
				if (!fairyScoredFor && playerHasFairy.at(i)) {
					fairyScoredFor = true;
					farmScore.at(i) += 3;
				}
			}
		}
	}
	return farmScore;
}

int Table::getMaxValue(vector<int> val) {
	int max = 0;
	for (int i = 0; i < val.size(); ++i) {
		if (val.at(i) > max) {
			max = val.at(i);
		}
	}
	return max;
}
