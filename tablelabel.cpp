#include "tablelabel.h"

TableLabel::TableLabel(QWidget *parent, Qt::WindowFlags f): QLabel(parent) {}

void TableLabel::mousePressEvent(QMouseEvent *event) {
	const QPoint p = event->pos();
	emit clicked(p);
}

void TableLabel::setPosition(int x, int y) {
	pos_x = x;
	pos_y = y;
}

void TableLabel::getPosition(int *x, int *y) {
	*x = pos_x;
	*y = pos_y;
}
