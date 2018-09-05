#ifndef DRAGONMOVER_H
#define DRAGONMOVER_H
#include <vector>
#include <algorithm>
#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>
#include <QScrollArea>
#include <QLabel>
#include <QString>
#include "table.h"
#include "piece.h"

using namespace std;

class DragonMover : public QDialog {
	Q_OBJECT

private:
	QVBoxLayout *layout;
	QGridLayout *buttonLayout;
	QPushButton *north;
	QPushButton *east;
	QPushButton *south;
	QPushButton *west;
	QLabel *label;
	QScrollArea *scroll;
	Table *table;
	Piece *dragon;
	int players;
	int currentPlayer;
	int moves;
	int size;
	int x;
	int y;
	vector<Tile*> dragonHistory;
	void update();

public:
	DragonMover(QWidget *parent, Table *t, Piece *d, QScrollArea *a, int ps, int p, int s);
	~DragonMover();

private slots:
	void buttonClicked();
};
#endif
