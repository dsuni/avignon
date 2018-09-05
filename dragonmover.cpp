#include "dragonmover.h"

DragonMover::DragonMover(QWidget *parent, Table *t, Piece *d, QScrollArea *a, int ps, int p, int s) : table(t), dragon(d), scroll(a), players(ps), currentPlayer(p), size(s) {
	moves = 0;
	layout = new QVBoxLayout(); // Deleted in destructor
	buttonLayout = new QGridLayout();
	label = new QLabel();
	north = new QPushButton("^");
	east = new QPushButton(">");
	south = new QPushButton("v");
	west = new QPushButton("<");
	this->setLayout(layout);
	layout->addWidget(label);
	layout->addLayout(buttonLayout);
	buttonLayout->addWidget(north, 0, 1);
	buttonLayout->addWidget(east, 1, 2);
	buttonLayout->addWidget(south, 2, 1);
	buttonLayout->addWidget(west, 1, 0);
	connect(north, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	connect(east, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	connect(south, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	connect(west, SIGNAL(clicked()), this, SLOT(buttonClicked()));
	this->show();
	update();
}

DragonMover::~DragonMover() {
	delete north;
	north = NULL;
	delete east;
	east = NULL;
	delete south;
	south = NULL;
	delete west;
	west = NULL;
	delete buttonLayout;
	buttonLayout = NULL;
	delete label;
	label = NULL;
	delete layout;
	layout = NULL;
}

void DragonMover::update() {
	north->setEnabled(false);
	east->setEnabled(false);
	south->setEnabled(false);
	west->setEnabled(false);
	QString str("Player ");
	str.append(QString::number(currentPlayer));
	str.append(": Please move the dragon.");
	label->setText(str);
	table->getDragon(&x, &y);
	dragonHistory.push_back(table->getTile(x, y));
	scroll->ensureWidgetVisible(table->getLabel(x, y), size, size);
	vector<Tile*> neighbors = table->getNeighbors(x, y);
	bool deadEnd = true;
	for (int i = 0; i < neighbors.size(); ++i) {
		// Can not move dragon to place that
		// a) is empty, or
		// b) has the fairy, or
		// c) the dragon has already visited during this set of moves
		if (neighbors.at(i) == NULL || neighbors.at(i)->hasFairy() || std::find(dragonHistory.begin(), dragonHistory.end(), neighbors.at(i)) != dragonHistory.end()) {
			continue;
		}
		deadEnd = false;
		switch(i) {
		case 0:
			north->setEnabled(true);
			break;
		case 1:
			east->setEnabled(true);
			break;
		case 2:
			south->setEnabled(true);
			break;
		case 3:
			west->setEnabled(true);
			break;
		default:
			break;
		}
	}
	if (deadEnd) {
		QMessageBox::information(this, "Dead end", "The dragon has reached a dead end.", QMessageBox::Ok);
		this->close();
	}
}

void DragonMover::buttonClicked() {
	QPushButton *button = (QPushButton*)QObject::sender();
	int oldX = x;
	int oldY = y;
	if (button == north) {
		--y;
	}
	else if (button == east) {
		++x;
	}
	else if (button == south) {
		++y;
	}
	else {
		--x;
	}
	table->setDragon(x, y);
	table->getTile(x, y)->getImageContainer()->setPiece(dragon);
	table->getTile(x, y)->getImageContainer()->removeFollowers();
	QPixmap *pxm = table->getTile(x, y)->getPixmap(size);
	table->getLabel(x, y)->setPixmap(*pxm);
	delete pxm;
	pxm = table->getTile(oldX, oldY)->getPixmap(size);
	table->getLabel(oldX, oldY)->setPixmap(*pxm);
	delete pxm;
	++moves;
	if (moves == 6) {
		QMessageBox::information(this, "Out of moves", "The dragon has moved 6 times and will now stop.", QMessageBox::Ok);
		this->close();
		return;
	}
	++currentPlayer;
	if (currentPlayer > players) {
		currentPlayer = 1;
	}
	update();
}
