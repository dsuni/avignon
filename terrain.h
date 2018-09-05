#ifndef TERRAIN_H
#define TERRAIN_H
#include <map>
#include <string>

using namespace std;

enum Terrain { undefined, field, road, city, river,
							 terrainFirst = undefined, terrainLast = river }; 
enum TerrainAddon { none, monastery, shield, intersection, pigs, inn, cathedral, volcano, wine, cloth, grain, princess, dragon, portal,
										terrainAddonFirst = none, terrainAddonLast = portal };

struct terrainMap : public map<string, Terrain> {
	terrainMap() {
		this->operator[]("undefined") = undefined;
		this->operator[]("field") = field;
		this->operator[]("road") = road;
		this->operator[]("city") = city;
		this->operator[]("river") = river;
	};
};

struct terrainAddonMap : public map<string, TerrainAddon> {
	terrainAddonMap() {
		this->operator[]("none") = none;
		this->operator[]("monastery") = monastery;
		this->operator[]("shield") = shield;
		this->operator[]("intersection") = intersection;
		this->operator[]("pigs") = pigs;
		this->operator[]("inn") = inn;
		this->operator[]("cathedral") = cathedral;
		this->operator[]("volcano") = volcano;
		this->operator[]("wine") = wine;
		this->operator[]("cloth") = cloth;
		this->operator[]("grain") = grain;
		this->operator[]("princess") = princess;
		this->operator[]("dragon") = dragon;
		this->operator[]("portal") = portal;
	};
};

#endif
