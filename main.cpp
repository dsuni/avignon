/*
avignon version 0.7.0, released 22 Sep 2018

Copyright 2018 Daniel Suni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "gamecreation.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);
	MainWindow mw;
	GameCreation gc;
	gc.show();
	QObject::connect(&gc, SIGNAL(startGame(int, int, int, int, QStringList*, QStringList*)), &mw, SLOT(start(int, int, int, int, QStringList*, QStringList*)));
	return app.exec();
}
