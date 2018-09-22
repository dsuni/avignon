#include "occupationmapper.h"

OccupationMapper::OccupationMapper(QString *baseDir) {
	renderers[not_applicable] = new QSvgRenderer();
	QString tempStr(*baseDir);
	tempStr.append("/graphics/knight.svg");
	renderers[knight] = new QSvgRenderer(tempStr);
	tempStr = *baseDir;
	tempStr.append("/graphics/robber.svg");
	renderers[robber] = new QSvgRenderer(tempStr);
	tempStr = *baseDir;
	tempStr.append("/graphics/farmer.svg");
	renderers[farmer] = new QSvgRenderer(tempStr);
	tempStr = *baseDir;
	tempStr.append("/graphics/monk.svg");
	renderers[monk] = new QSvgRenderer(tempStr);
}

OccupationMapper::~OccupationMapper() {
	delete renderers[not_applicable];
	delete renderers[knight];
	delete renderers[robber];
	delete renderers[farmer];
	delete renderers[monk];
}

QSvgRenderer* OccupationMapper::getRenderer(Occupation o) {
	return renderers[o];
}
