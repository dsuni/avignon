#ifndef OCCUPATIONMAPPER_H
#define OCCUPATIONMAPPER_H
#include <map>
#include <QSvgRenderer>

using namespace std;

enum Occupation { not_applicable, knight, robber, farmer, monk };

class OccupationMapper {
private:
	map<Occupation, QSvgRenderer*> renderers;

public:
	OccupationMapper();
	~OccupationMapper();
	QSvgRenderer* getRenderer(Occupation o);
};
#endif
