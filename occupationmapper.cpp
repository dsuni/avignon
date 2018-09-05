#include "occupationmapper.h"

OccupationMapper::OccupationMapper() {
	renderers[not_applicable] = new QSvgRenderer();
	renderers[knight] = new QSvgRenderer(QObject::tr("graphics/knight.svg"));
	renderers[robber] = new QSvgRenderer(QObject::tr("graphics/robber.svg"));
	renderers[farmer] = new QSvgRenderer(QObject::tr("graphics/farmer.svg"));
	renderers[monk] = new QSvgRenderer(QObject::tr("graphics/monk.svg"));
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
