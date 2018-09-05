#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) {
	table = NULL;
	deck = NULL;
	fairyPiece = NULL;
	dragonPiece = NULL;
	drawn = -1;
	turn = -1;
	active = false;
	gameOver = false;
	builderRedraw = false;
	builderPlaced = false;
	innsAndCathedrals = false;
	tradersAndBuilders = false;
	princessAndDragon = false;
	volcanoPlayed = false;
	portalActive = false;
	placeFairy = false;
	occupationMapper = new OccupationMapper(); // Deleted in destructor
	QRect rec = QApplication::desktop()->screenGeometry();
	zoom = 4;
	if (rec.height() < 500) {
		zoom = 2;
	}
	else if (rec.height() < 1200) {
		zoom = 3;
	}
	screenSize = zoom;
}

MainWindow::~MainWindow() {
	delete table;
	table = NULL;
	delete deck;
	deck = NULL;
	delete occupationMapper;
	occupationMapper = NULL;
	for (int i = 0; i < merchandiseLabels.size(); ++i) {
		delete merchandiseLabels.at(i);
	}
	delete backDrawPile;
	backDrawPile = NULL;
	delete backTable;
	backTable = NULL;
	delete barrelImg;
	barrelImg = NULL;
	delete barleyImg;
	barleyImg = NULL;
	delete clothImg;
	clothImg = NULL;
	delete fairyPiece;
	fairyPiece = NULL;
	delete dragonPiece;
	dragonPiece = NULL;
	delete window;
	window = NULL;
}

void MainWindow::start(int x, int y, int p, int r, QStringList *decks, QStringList *rulesList) {
	// Set up rules
	innsAndCathedrals = rulesList->contains(tr("inns_and_cathedrals.xml"));
	tradersAndBuilders = rulesList->contains(tr("traders_and_builders.xml"));
	princessAndDragon = rulesList->contains(tr("the_princess_and_the_dragon.xml"));
	if (princessAndDragon) {
		dragonPiece = new Piece(tr("graphics/dragon_piece.svg"), 50, 50, 0.8); // Deleted in destructor
		fairyPiece = new Piece(tr("graphics/fairy.svg"), 150, 150, 0.2); // Deleted in destructor
	}
	delete rulesList;
	// Qt's radiobutton index starts from -2 and decreases (WTF?)
	// This way 1st ed rules -> 1, 2nd ed rules -> 2, et.c.
	rules = -1 * r - 1;
	window = new QWidget(); // Deleted in destructor
	// Set up players
	QStringList colors;
	colors << tr("Red") << tr("Blue") << tr("Green") << tr("Yellow") << tr("Black") << tr("Grey");
	for (int i = 0; i < p; ++i) {
		QString text = "Pick a color for player ";
		text.append(QString::number(i + 1));
		bool ok;
		QString color;
		do {
			color = QInputDialog::getItem(this, tr("Choose color"), text, colors, 0, false, &ok);
		} while (!ok || color.isEmpty());
		for (int j = 0; j < colors.size(); ++j) {
			if (color == colors.at(j)) {
				colors.removeAt(j);
				break;
			}
		}
		players.push_back(new Player(i, color.toLower().toStdString()));
		players.at(i)->addFollower(normal, occupationMapper, 7);
		if (innsAndCathedrals) {
			players.at(i)->addFollower(big, occupationMapper, 1);
		}
		if (tradersAndBuilders) {
			players.at(i)->addFollower(builder, occupationMapper, 1);
			players.at(i)->addFollower(pig, occupationMapper, 1);
		}
		scoreWidgets.push_back(new QWidget(window));
		scoreLayouts.push_back(new QVBoxLayout(window));
		merchandiseLayouts.push_back(new QHBoxLayout(window));
		merchandiseLayouts.at(i)->setAlignment(Qt::AlignLeft);
		scoreLabels.push_back(new QLabel(window));
		merchandiseLabels.push_back(new vector<QLabel*>()); // Deleted in destructor
		scoreWidgets.at(i)->setLayout(scoreLayouts.at(i));
		scoreLayouts.at(i)->addWidget(scoreLabels.at(i));
		scoreLayouts.at(i)->addLayout(merchandiseLayouts.at(i));
		QString score = tr("Player ");
		score.append(QString::number(i + 1));
		score.append(tr(" score: 0"));
		scoreLabels.at(i)->setText(score);
		scoreLabels.at(i)->setAlignment(Qt::AlignTop);
	}
	// Set up deck
	table = new Table(x, y); // Deleted in destructor
	table->setExpansions(innsAndCathedrals, princessAndDragon);
	deck = new Deck(); // Deleted in destructor
	bool core = decks->contains("core.xml");
	bool river = decks->contains("river.xml");
	bool river2 = decks->contains("river_II.xml");
	int mem = 0;
	// Load order matters when the river expansions are involved. We want to end
	// up with the spring tile first, the appropriate amount of lake tiles last,
	// and the tiles in between mixed - but only amongst themselves.
	if (river2) {
		if (river) {
			deck->loadTiles(tr("decks/river.xml"));
			// Erase last (==lake) tile of river expansion when using both.
			mem = deck->getSize();
			deck->eraseTile(mem - 1);
		}
		deck->loadTiles(tr("decks/river_II.xml"));
		// If loading both, we then need to erase the first (==spring) tile of
		// the river II deck, which is now where the lake tile was before deletion.
		// Then we swap the second tile with the river fork tile.
		if (river) {
			deck->eraseTile(mem - 1);
			deck->swapTiles(1, mem - 1);
		}
		// Leave the two first (spring/fork) and two last tiles (lakes) in place. Shuffle the rest.
		deck->shuffleDeck(2, deck->getSize() - 3);
		mem = deck->getSize();
	}
	else if (river) {
		deck->loadTiles(tr("decks/river.xml"));
		// When using river only, shuffle all between 1st and last.
		deck->shuffleDeck(1, deck->getSize() - 2);
		mem = deck->getSize();
	}
	// If neither expansion is loaded we load core first because of starting tile.
	else if (core) {
		deck->loadTiles(tr("decks/core.xml"));
	}
	for (int i = 0; i < decks->size(); ++i) {
		// These are always handled above.
		if (decks->at(i) == "river.xml" || decks->at(i) == "river_II.xml") {
			continue;
		}
		// Core is handled iff neither river expansion is loaded
		if (decks->at(i) == "core.xml" && !river && !river2) {
			continue;
		}
		QString deckDir("decks/");
		deck->loadTiles(deckDir.append(decks->at(i)));
	}
	// Shuffle non-river pieces among themselves
	if (river || river2) {
		deck->shuffleDeck(mem, deck->getSize() - 1);
	}
	// Shuffle all but first tile
	else {
		deck->shuffleDeck(1);
	}
	for (int i = 0; i < deck->getSize();++i) {
		deck->getTile(i)->launchSetup();
	}
	delete decks;
	decks = NULL;
	rotateButtons = new QWidget(window);
	zoomButtons = new QWidget(window);
	mainLayout = new QHBoxLayout(window);
	rotateLayout = new QHBoxLayout(window);
	zoomLayout = new QHBoxLayout(window);
	tableLayout = new QGridLayout(window);
	tableLayout->setHorizontalSpacing(0);
	tableLayout->setVerticalSpacing(0);
	controlLayout = new QVBoxLayout(window);
	currentDraw = new QLabel(window);
	tileCounter = new QLabel(window);
	// Tile backs image
	QSvgRenderer renderer(tr("graphics/tile_back.svg"));
	QImage img(tileSizes[zoom], tileSizes[zoom], IMG_FORMAT);
	QPainter painter(&img);
	renderer.render(&painter);
	backTable = new QPixmap(QPixmap::fromImage(img)); // Deleted in destructor
	backDrawPile = new QPixmap(QPixmap::fromImage(img)); // Deleted in destructor
	currentDraw->setPixmap(*backDrawPile);
	currentDraw->setAlignment(Qt::AlignTop);
	// Merchandise images
	QSvgRenderer renderer1(tr("graphics/barrel.svg"));
	QImage img1(20, 20, IMG_FORMAT);
	QPainter painter1(&img1);
	renderer1.render(&painter1);
	barrelImg = new QPixmap(QPixmap::fromImage(img1)); // Deleted in destructor
	QSvgRenderer renderer2(tr("graphics/barley.svg"));
	QImage img2(20, 20, IMG_FORMAT);
	QPainter painter2(&img2);
	renderer2.render(&painter2);
	barleyImg = new QPixmap(QPixmap::fromImage(img2)); // Deleted in destructor
	QSvgRenderer renderer3(tr("graphics/cloth.svg"));
	QImage img3(20, 20, IMG_FORMAT);
	QPainter painter3(&img3);
	renderer3.render(&painter3);
	clothImg = new QPixmap(QPixmap::fromImage(img3)); // Deleted in destructor
	// UI setup
	controlLayout->addWidget(currentDraw);
	tileCounter->setAlignment(Qt::AlignTop);
	tileCounter->setAlignment(Qt::AlignHCenter);
	tileCounter->setText(QString::number(deck->getSize()));
	controlLayout->addWidget(tileCounter);
	for (int i = 0; i < scoreWidgets.size(); ++i) {
		controlLayout->addWidget(scoreWidgets.at(i));
	}
	tableArea = new QWidget(window);
	tableArea->setLayout(tableLayout);
	scroll = new QScrollArea(window);
	scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scroll->setWidgetResizable(true);
	for (int i = 0; i < y; ++i) {
		for (int j = 0; j < x; ++j) {
			tableLayout->addWidget(table->getLabel(j, i), i, j);
			table->setPixmap(j, i, backTable);
			connect(table->getLabel(j, i), SIGNAL(clicked(QPoint)), this, SLOT(selectedTableLabel()));
		}
	}
	tableLayout->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	draw = new QPushButton(tr("Draw tile"), window);
	connect(draw, SIGNAL(clicked()), this, SLOT(drawTile()));
	rotateCW = new QPushButton(window);
	QPixmap rotateCWPxm("graphics/rotate_cw.svg");
	QIcon rotateCWIcon(rotateCWPxm);
	rotateCW->setIcon(rotateCWIcon);
	rotateCW->setEnabled(false);
	connect(rotateCW, SIGNAL(clicked()), this, SLOT(rotateClockwise()));	
	rotateCCW = new QPushButton(window);
	QPixmap rotateCCWPxm("graphics/rotate_ccw.svg");
	QIcon rotateCCWIcon(rotateCCWPxm);
	rotateCCW->setIcon(rotateCCWIcon);
	rotateCCW->setEnabled(false);
	connect(rotateCCW, SIGNAL(clicked()), this, SLOT(rotateCounterClockwise()));
	zoomInButton = new QPushButton(window);
	QPixmap zoomInPxm("graphics/zoom_in.svg");
	QIcon zoomInIcon(zoomInPxm);
	zoomInButton->setIcon(zoomInIcon);
	connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));
	zoomOutButton = new QPushButton(window);
	QPixmap zoomOutPxm("graphics/zoom_out.svg");
	QIcon zoomOutIcon(zoomOutPxm);
	zoomOutButton->setIcon(zoomOutIcon);
	connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));
	controlLayout->addStretch();
	controlLayout->addWidget(draw);
	rotateButtons->setLayout(rotateLayout);
	rotateLayout->addWidget(rotateCCW);
	rotateLayout->addWidget(rotateCW);
	controlLayout->addWidget(rotateButtons);
	zoomButtons->setLayout(zoomLayout);
	zoomLayout->addWidget(zoomOutButton);
	zoomLayout->addWidget(zoomInButton);
	controlLayout->addWidget(zoomButtons);
	window->setLayout(mainLayout);
	scroll->setWidget(tableArea);
	mainLayout->addWidget(scroll);
	mainLayout->addLayout(controlLayout);
	setCentralWidget(window);
	advanceTurn();
	this->show();
}

void MainWindow::drawTile() {
	draw->setEnabled(false);
	rotateCW->setEnabled(true);
	rotateCCW->setEnabled(true);
	++drawn;
	if (drawn == deck->getSize() - 1) {
		gameOver = true;
	}
	QPixmap *pxm = deck->getTile(drawn)->getPixmap(tileSizes[screenSize]);
	currentDraw->setPixmap(*pxm);
	delete pxm;
	tileCounter->setText(QString::number(deck->getSize() - drawn - 1));
	while (princessAndDragon && !volcanoPlayed && deck->getTile(drawn)->hasTerrainAddon(dragon)) {
		QMessageBox::warning(this, "Dragon not in play", "The dragon is not yet in play.\nThe tile will be shuffled into the deck, and a new one drawn.", QMessageBox::Ok);
		deck->mixIntoDeck(drawn);
		pxm = deck->getTile(drawn)->getPixmap(tileSizes[screenSize]);
		currentDraw->setPixmap(*pxm);
		delete pxm;
	}
	if (drawn > 0 && !legalMovesExist(deck->getTile(drawn))) {
		QMessageBox::warning(this, "No legal moves", "This tile can not be placed anywhere.\nIt will be discarded, and a new one drawn.", QMessageBox::Ok);
		drawTile();
	}
	else {
		active = true;
	}
}

bool MainWindow::legalMovesExist(Tile *t) {
	int h = table->getHeight();
	int w = table->getWidth();
	int mask = 255;
	int x, y, bothXY;
	bool legal = false;
	set<int>::iterator b = validPlacePositions.begin();
	set<int>::iterator e = validPlacePositions.end();
	if (deck->getTile(drawn)->hasTerrain(river)) {
		b = validRiverPositions.begin();
		e = validRiverPositions.end();
	}
	for (set<int>::iterator it = b; it != e; ++it) {
		bothXY = *it;
		x = bothXY >> 8;
		y = bothXY & mask;
		for (int i = 0; i < 4; ++i) {
			if (table->tileFits(t, x, y)) {
				legal = true;
			}
			t->rotateClockwise();
		}
		if (legal) {
			return true;
		}
	}
	return false;
}

void MainWindow::selectedTableLabel() {
	currentLabel = (TableLabel*)QObject::sender();
	int x, y;
	currentLabel->getPosition(&x, &y);
	QPixmap *pxm;
	vector<Follower*> evict;
	QMessageBox::StandardButton reply = QMessageBox::No;
	// First check if a portal is letting the user choose where to add a follower
	if (portalActive) {
		Tile *t = table->getTile(x, y);
		if (t == NULL) {
			QMessageBox::critical(this, "Illegal move", "Can not place follower on empty tile.", QMessageBox::Ok);
			return;
		}
		if (t->hasDragon()) {
			QMessageBox::critical(this, "Illegal move", "DO NOT FEED THE DRAGON!", QMessageBox::Ok);
			return;
		}
		placingTile = t;
		goto portalTileSelected;
	}
	// Then check if we're actually placing the fairy
	if (placeFairy) {
		Tile *t = table->getTile(x, y);
		if (t == NULL) {
			QMessageBox::critical(this, "Illegal move", "Can not place fairy on empty tile.", QMessageBox::Ok);
			return;
		}
		Follower *f;
		vector<Follower*> fs;
		for (int i = 0; i < 16; ++i) {
			f = t->getSubtile(i)->getFollower();
			if (f != NULL && f->getParent() == players.at(turn)) {
				fs.push_back(f);
			}
		}
		if (fs.size() == 0) {
			QMessageBox::critical(this, "Illegal move", "You can only place fairy where you already have a follower.", QMessageBox::Ok);
			return;
		}
		if (fs.size() == 1) {
			f = fs.at(0);
		}
		else {
			f = haveUserPickFollower(fs, false);
		}
		for (int i = 0; i < players.size(); ++i) {
			vector<Follower*> playerFollowers = players.at(i)->getAllFollowers();
			for (int j = 0; j < playerFollowers.size(); ++j) {
				playerFollowers.at(j)->setFairy(false);
			}
		}
		f->setFairy(true);
		int faX, faY;
		table->getFairy(&faX, &faY);
		table->setFairy(x, y);
		vector<Subtile*> sTiles = f->getPlacement()->getConnections();
		Subtile *s = f->getPlacement();
		if (!sTiles.empty() && sTiles.at(0)->getParent() == t) {
			s = sTiles.at(0);
		}
		int tileX, tileY;
		f->translateSubtileToCoordinates(t->getFairyPosition(s->getPosition()), &tileX, &tileY);
		fairyPiece->setPosition(tileX, tileY);
		fairyPiece->setSubtile(s);
		t->getImageContainer()->setPiece(fairyPiece);
		pxm = t->getPixmap(tileSizes[zoom]);
		currentLabel->setPixmap(*pxm);
		delete pxm;
		if (faX >= 0) {
			pxm = table->getTile(faX, faY)->getPixmap(tileSizes[zoom]);
			table->getLabel(faX, faY)->setPixmap(*pxm);
			delete pxm;
		}
		placeFairy = false;
		checkFeatureCompletion();
		return;
	}
	// If we're not in the mode of placing a tile, do nothing.
	if (!active) {
		return;
	}
	if (drawn == 0) {
		updateValidPositions(x, y, true);
	}
	// Only the first tile may be placed in a space with no neighboring tiles.
	else if (!table->hasNeighbors(x, y)) {
		return;
	}
	// Only legal placements are allowed.
	else if (!isValidPosition(x, y)) {
		return;
	}
	// Tile must fit
	else if (!table->tileFits(deck->getTile(drawn), x, y)) {
		return;
	}
	// River U-turns not allowed
	else if (riverDoesUTurn(x, y)) {
		QMessageBox::critical(this, "Illegal move", "River U-turns are not allowed.", QMessageBox::Ok);
		return;
	}
	else {
		updateValidPositions(x, y);
	}
	active = false;
	rotateCW->setEnabled(false);
	rotateCCW->setEnabled(false);
	table->addTile(deck->getTile(drawn), x, y);
	deck->getTile(drawn)->setPosition(x, y);
	pxm = deck->getTile(drawn)->getPixmap(tileSizes[zoom]);
	currentLabel->setPixmap(*pxm);
	delete pxm;
	currentDraw->setPixmap(*backDrawPile);
	// Check if we need to remove a follower
	evict = followersToEvict();
	if (princessAndDragon && !evict.empty()) {
		Follower *evictee;
		if (evict.size() == 1) {
			evictee = *(evict.begin());
		}
		else {
			evictee = haveUserPickFollower(evict, true);
		}
		restoreFollowerToPlayer(evictee);
		checkFeatureCompletion();
		return;
	}
	// We don't get to place a follower if we drew a volcano tile while
	// playing the princess & the dragon expansion
	else if (princessAndDragon && deck->getTile(drawn)->hasTerrainAddon(volcano)) {
		int drX, drY;
		table->getDragon(&drX, &drY);
		volcanoPlayed = true;
		table->setDragon(x, y);
		deck->getTile(drawn)->getImageContainer()->setPiece(dragonPiece);
		pxm = deck->getTile(drawn)->getPixmap(tileSizes[zoom]);
		currentLabel->setPixmap(*pxm);
		delete pxm;
		if (drX >= 0) {
			pxm = table->getTile(drX, drY)->getPixmap(tileSizes[zoom]);
			table->getLabel(drX, drY)->setPixmap(*pxm);
			delete pxm;
		}
	}
	// If princess and dragon rules we also want to ask about the fairy
	else if (princessAndDragon && canPlaceFairy()) {
		QMessageBox msgBox;
		QAbstractButton *followerBtn;
		msgBox.setWindowTitle(tr("Placing"));
		if (players.at(turn)->hasPlayableFollowers()) {
			msgBox.setText(tr("Do you wish to place a follower, place the fairy or do nothing?"));
			followerBtn = msgBox.addButton(tr("Place follower"), QMessageBox::YesRole);
		}
		else {
			msgBox.setText(tr("Do you wish to place the fairy or do nothing?"));
		}
		QAbstractButton *fairyBtn = msgBox.addButton(tr("Place fairy"), QMessageBox::NoRole);
		QAbstractButton *rejectBtn = msgBox.addButton(tr("Do nothing"), QMessageBox::AcceptRole);
		msgBox.exec();
		if (msgBox.clickedButton() == fairyBtn) {
			placeFairy = true;
			QMessageBox::information(this, "Fairy", "Please select tile to place fairy on.", QMessageBox::Ok);
			return;
		}
		else if (msgBox.clickedButton() == followerBtn) {
			reply = QMessageBox::Yes;
		}
	}
	// Ask about follower placing
	else if (players.at(turn)->hasPlayableFollowers()) {
		reply = QMessageBox::question(this, "Placing", "Do you wish to place a follower?", QMessageBox::Yes|QMessageBox::No);
	}
	if (reply == QMessageBox::Yes) {
		if (princessAndDragon && deck->getTile(drawn)->hasTerrainAddon(portal)) {
			portalActive = true;
			QMessageBox::information(this, "Portal", "Portal allows you to select another tile where to place your follower.", QMessageBox::Ok);
		}
		placer = new FollowerPlacer(this, portalActive); // Deleted in placeFollower
		connect(placer, SIGNAL(followerTypeSelected(FollowerType)), this, SLOT(setFollowerType(FollowerType)));
		connect(placer, SIGNAL(keepPortalActive()), this, SLOT(keepPortalActive()));
		connect(placer->getLabel(), SIGNAL(clicked(QPoint)), this, SLOT(placeFollower(QPoint)));
		placer->setFollowers(players.at(turn)->getAllFollowers());
		if (portalActive) {
			return;
		}
		else {
			placingTile = deck->getTile(drawn);
		}
	portalTileSelected:
		portalActive = false;
		pxm = placingTile->getPixmap();
		placer->setImage(pxm);
		delete pxm;
		placer->exec();
		if (portalActive) {
			return;
		}
	}
	checkFeatureCompletion();
}

void MainWindow::checkFeatureCompletion() {
	// The dragon moves (if applicable) before feature completion check
	if (princessAndDragon && volcanoPlayed && deck->getTile(drawn)->hasTerrainAddon(dragon)) {
		DragonMover dragonMover(this, table, dragonPiece, scroll, players.size(), turn + 1, tileSizes[zoom]);
		dragonMover.exec();
	}
	// Check monastery completion
	vector<Tile*> neighbors = table->getMonasteryNeighbors(deck->getTile(drawn));
	for (int i = 0; i < neighbors.size(); ++i) {
		if (neighbors.at(i) != NULL && neighbors.at(i)->hasTerrainAddon(monastery) && table->isMonasteryComplete(neighbors.at(i))) {
			Follower *temp = neighbors.at(i)->getMonk();
			if (temp == NULL) {
				continue;
			}
			if (temp->hasFairy()) {
				addScore(3, temp->getParent());
			}
			restoreFollowerToPlayer(temp);
			addScore(9, temp->getParent());
		}
	}
	vector<Subtile*> subs = deck->getTile(drawn)->getDistinctFeatures();
	set<Follower*> followers;
	// Check if we need to do one more turn
	if (!builderRedraw && !builderPlaced && !gameOver) {
		for (int i = 0; i < subs.size(); ++i) {
			if (subs.at(i)->getTerrain() == city || subs.at(i)->getTerrain() == road) {
				followers= table->getAllFollowersInFeature(subs.at(i));
				for (set<Follower*>::iterator it=followers.begin(); it!=followers.end(); ++it) {
					Follower *temp = *it;
					if (temp->getType() == builder && temp->getParent()->getPlayerNumber() == turn) {
						builderRedraw = true;
						break;
					}
				}
			}
		}
	}
	else {
		builderRedraw = false;
	}
	// Check road & city completion
	for (int i = 0; i < subs.size(); ++i) {
		// Monasteries are checked elsewhere.
		if (subs.at(i)->getTerrainAddon() == monastery) {
			continue;
		}
		if ((subs.at(i)->getTerrain() == road && table->isRoadComplete(subs.at(i))) || (subs.at(i)->getTerrain() == city && table->isCityComplete(subs.at(i)))) {
			followers = table->getAllFollowersInFeature(subs.at(i));
			vector<int> playerWeight;
			vector<bool> playerHasFairy;
			playerWeight.assign(players.size(), 0);
			playerHasFairy.assign(players.size(), false);
			for (set<Follower*>::iterator it=followers.begin(); it!=followers.end(); ++it) {
				Follower *temp = *it;
				switch(temp->getType()) {
				case big:
					++playerWeight.at(temp->getParent()->getPlayerNumber());
				case normal:
					++playerWeight.at(temp->getParent()->getPlayerNumber());
					if (temp->hasFairy()) {
						playerHasFairy.at(temp->getParent()->getPlayerNumber()) = true;
					}
					break;
				default:
					break;
				}
				restoreFollowerToPlayer(temp);
			}
			int max = getMaxValue(playerWeight);
			int score = table->getFeatureScore(subs.at(i), rules);
			if (tradersAndBuilders) {
				int barrels, barley, spools;
				table->getMerchandise(&barrels, &barley, &spools);
				addMerchandise(barrels, barley, spools);
			}
			if (max > 0) {
				for (int j = 0; j < playerWeight.size(); ++j) {
					if (playerWeight.at(j) == max) {
						addScore(score, j);
						if (playerHasFairy.at(j)) {
							addScore(3, j);
						}
					}
				}
			}
	  }
	}
	if (!builderRedraw) {
		advanceTurn();
	}
	else {
		draw->setEnabled(true);
	}
}

void MainWindow::keepPortalActive() {
	portalActive = true;
}

int MainWindow::getMaxValue(vector<int> v) {
	int max = 0;
	for (int i = 0; i < v.size(); ++i) {
		if (v.at(i) > max) {
			max = v.at(i);
		}
	}
	return max;
}

void MainWindow::rotateClockwise() {
	deck->getTile(drawn)->rotateClockwise();
	QPixmap *pxm = deck->getTile(drawn)->getPixmap(tileSizes[screenSize]);
	currentDraw->setPixmap(*pxm);
	delete pxm;
}

void MainWindow::rotateCounterClockwise() {
	deck->getTile(drawn)->rotateCounterClockwise();
	QPixmap *pxm = deck->getTile(drawn)->getPixmap(tileSizes[screenSize]);
	currentDraw->setPixmap(*pxm);
	delete pxm;
}

void MainWindow::zoomIn() {
	if (zoom < 5) {
		int visible = 0;
		vector<TableLabel*> labels = table->getLabels();
		vector<Tile*> tiles = table->getTiles();
		for (int i = 0; i < labels.size() ; ++i) {
			if (tiles.at(i) != NULL) {
				// First we get the first tile that's not empty...
				if (visible == 0) {
					visible = i;
				}
				// ...but what we really want is the first *visible* tile that's not empty - if it exists.
				if (!labels.at(i)->visibleRegion().isEmpty()) {
					visible = i;
					break;
				}
			}
		}
		++zoom;
		redrawTable();
		// redrawTable() already does the qApp->processEvents(), but for reasons known by Cthulhu,
		// Sauron and maybe some other entities of ancient evil, it won't actually work unless done
		// again here... so we do it again here. :-P
		qApp->processEvents();
		scroll->ensureWidgetVisible(labels.at(visible), tileSizes[zoom], tileSizes[zoom]);
	}
}

void MainWindow::zoomOut() {
	if (zoom > 0) {
		int visible = 0;
		vector<TableLabel*> labels = table->getLabels();
		vector<Tile*> tiles = table->getTiles();
		for (int i = 0; i < labels.size() ; ++i) {
			if (tiles.at(i) != NULL) {
				if (visible == 0) {
					visible = i;
				}
				if (!labels.at(i)->visibleRegion().isEmpty()) {
					visible = i;
					break;
				}
			}
		}
		--zoom;
		redrawTable();
		qApp->processEvents();
		scroll->ensureWidgetVisible(labels.at(visible), tileSizes[zoom], tileSizes[zoom]);
	}
}

void MainWindow::restoreFollowerToPlayer(Follower *f) {
	if (f == NULL) {
		return;
	}
	Tile *t = f->getPlacement()->getParent();
	t->getImageContainer()->removeFollower(f);
	f->getPlacement()->setFollower(NULL);
	QPixmap *pxm = t->getPixmap(tileSizes[zoom]);
	TableLabel *tl = table->getLabel(t->getX(), t->getY());
	tl->setPixmap(*pxm);
	delete pxm;
	f->setPlacement(NULL);
}

void MainWindow::redrawTable() {
	QSvgRenderer renderer(tr("graphics/tile_back.svg"));
	QImage img(tileSizes[zoom], tileSizes[zoom], IMG_FORMAT);
	QPainter painter(&img);
	renderer.render(&painter);
	delete backTable;
	backTable = new QPixmap(QPixmap::fromImage(img));	
	vector<Tile*> tiles = table->getTiles();
	vector<TableLabel*> labels = table->getLabels();
	for (int i = 0; i < tiles.size(); ++i) {
		if (tiles.at(i) == NULL) {
			labels.at(i)->setPixmap(*backTable);
		}
		else {
			QPixmap *pxm = tiles.at(i)->getPixmap(tileSizes[zoom]);
			labels.at(i)->setPixmap(*pxm);
			delete pxm;
		}
	}
	qApp->processEvents();
}

void MainWindow::advanceTurn() {
	builderPlaced = false;
	++turn;
	if (turn == players.size()) {
		turn = 0;
	}
	for (int i = 0; i < scoreLabels.size(); ++i) {
		QString style = "color: ";
		style.append(htmlColorMap()[players.at(i)->getColor()].c_str());
		style.append("; border: 0px");
		if (i == turn) {
			scoreWidgets.at(i)->setStyleSheet(tr("border: 5px solid black"));
			for (int j = 0; j < merchandiseLabels.at(i)->size(); ++j) {
				merchandiseLabels.at(i)->at(j)->setStyleSheet(tr("border: 0px"));
			}
		}
		else {
			scoreWidgets.at(i)->setStyleSheet(tr("border: 0px"));
		}
		scoreLabels.at(i)->setStyleSheet(style);
	}
	if (gameOver) {
		countFinalScore();
	}
	else {
		// Award a point to a player who starts with the fairy.
		if (princessAndDragon) {
			vector<Follower*> followers = players.at(turn)->getAllFollowers();
			for (int i = 0; i < followers.size(); ++i) {
				if (followers.at(i)->hasFairy()) {
					addScore(1, turn);
					break;
				}
			}
		}
		draw->setEnabled(true);
	}
}

void MainWindow::placeFollower(const QPoint p) {
	int subnum = translatePointToSubtile(p.x(), p.y(), IMG_DEFAULT_SIZE);
	Subtile *subtile = placingTile->getSubtile(subnum);
	bool found = false;
	set<Follower*> followers;
	switch(followerType) {
	case normal:
	case big:
		if (table->doesFeatureContainFollowers(subtile, true)) {
			QMessageBox::critical(this, "Illegal move", "Illegal move. That feature already has a follower.", QMessageBox::Ok);
			placer->raise();
			return;
		}
		break;
	case builder:
		if (subtile->getTerrain() != city && subtile->getTerrain() != road) {
			QMessageBox::critical(this, "Illegal move", "Illegal move. Builders may only be added to cities and roads.", QMessageBox::Ok);
			placer->raise();
			return;
		}
		followers = table->getAllFollowersInFeature(subtile);
		for (set<Follower*>::iterator it = followers.begin(); it != followers.end(); ++it) {
			Follower *f = *it;
			if (f->getParent()->getPlayerNumber() == turn) {
				found = true;
				break;
			}
		}
		if (!found) {
			QMessageBox::critical(this, "Illegal move", "Illegal move. Builders may only be added where you already have a follower.", QMessageBox::Ok);
			placer->raise();
			return;
		}
		break;
	case pig:
		if (subtile->getTerrain() != field) {
			QMessageBox::critical(this, "Illegal move", "Illegal move. Pigs may only be added to fields.", QMessageBox::Ok);
			placer->raise();
			return;
		}
		followers = table->getAllFollowersInFeature(subtile);
		for (set<Follower*>::iterator it = followers.begin(); it != followers.end(); ++it) {
			Follower *f = *it;
			if (f->getParent()->getPlayerNumber() == turn) {
				found = true;
				break;
			}
		}
		if (!found) {
			QMessageBox::critical(this, "Illegal move", "Pigs may only be added where you already have a follower.", QMessageBox::Ok);
			placer->raise();
			return;
		}
		break;
	}
	// If the placing is done via portal on some other tile than the one just
	// drawn, we need to check for some further possible illegal moves
	if (placingTile != deck->getTile(drawn)) {
		if (subtile->getTerrainAddon() == monastery) {
			if (table->isMonasteryComplete(placingTile)) {
				QMessageBox::critical(this, "Illegal move", "You can not add a follower to a feature that has already been completed.", QMessageBox::Ok);
				placer->raise();
				return;
			}
		}
		else if ((subtile->getTerrain() == city && table->isCityComplete(subtile)) ||
				(subtile->getTerrain() == road && table->isRoadComplete(subtile))) {
			QMessageBox::critical(this, "Illegal move", "You can not add a follower to a feature that has already been completed.", QMessageBox::Ok);
			placer->raise();
			return;
		}
	}
	QString message = tr("Confirm that you want to add ");
	if (subtile->getTerrainAddon() == monastery) {
		message.append("a monk to the monastery.");
	}
	else {
		switch (subtile->getTerrain()) {
		case road:
			if (followerType == builder) {
				message.append("a builder to the road.");
			}
			else {
				message.append("a bandit to the road.");
			}
			break;
		case city:
			if (followerType == builder) {
				message.append("a builder to the city.");
			}
			else {
				message.append("a knight to the city.");
			}
			break;
		case field:
			if (followerType == pig) {
				message.append("a pig to the field.");
			}
			else {
				message.append("a farmer to the field.");
			}
			break;
		default:
			QMessageBox::warning(this, tr("Invalid terrain"), tr("You can not place a follower there."), QMessageBox::Ok);
			placer->raise();
			return;
		}
	}
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, tr("Confirmation request"), message, QMessageBox::Ok|QMessageBox::Cancel);
	if (reply == QMessageBox::Cancel) {
		placer->raise();
		return;
	}
	placer->close();
	delete placer;
	placer = NULL;
	if (followerType == builder) {
		builderPlaced = true;
	}
	Follower *f = players.at(turn)->getFollower(followerType);
	subtile->setFollower(f);
	f->setPlacement(subtile);
	// Are we adding the follower to a tile/feature where the fairy already is?
	if (princessAndDragon && subtile->isOnSameFeatureAndTile(fairyPiece->getSubtile())) {
		f->setFairy(true);
	}
	placingTile->getImageContainer()->addFollower(f);
	QPixmap *pxm = placingTile->getPixmap(tileSizes[zoom]);
	currentLabel->setPixmap(*pxm);
	delete pxm;
}

// If this makes no sense, stare at the ascii diagram in tile.h
// until it does (or your eyes start to bleed, whichever comes first)
int MainWindow::translatePointToSubtile(int x, int y, int size) {
	int third = size / 3;
	int twoThirds = 2 * size / 3;
	if (x < third) { // Leftmost third
		if (y < third) { // Upper left corner
			if (x > y) { // Upper right triangle
				return 0;
			}
			else { // Lower left triangle
				return 11;
			}
		}
		else if (y < twoThirds) { // Middle left
			return 10;
		}
		else { // Lower left corner
			if (x + y - twoThirds > third) { // Right lower triangle
				return 8;
			}
			else {
				return 9;
			}
		}
	}
	else if (x < twoThirds) { // Middle column
		if (y < third) { // Upper area
			return 1;
		}
		else if (y > twoThirds) { // Lower area
			return 7;
		}
		else { // Absolute center
			if (x > y) { // Upper right
				if (x + y > size) { // Right
					return 13;
				}
				else {
					return 12; // Upper
				}
			}
			else { // Lower left
				if (x + y > size) { // Lower
					return 14;
				}
				else { // Left
					return 15;
				}
			}
		}
	}
	else { // Rightmost third
		if (y < third) { // Upper right
			if (x - twoThirds + y > third) { // Right triangle
				return 3;
			}
			else { // Left triangle
				return 2;
			}
		}
		else if (y < twoThirds) { // Middle right
			return 4;
		}
		else { // Lower right
			if (x > y) { // Right triangle
				return 5;
			}
			else { // Left triangle
				return 6;
			}
		}
	}
}

void MainWindow::addScore(int score, int player) {
	players.at(player)->addPoints(score);
	QString pts(tr("Player "));
	pts.append(QString::number(player + 1));
	pts.append(tr(" score: "));
	pts.append(QString::number(players.at(player)->getPoints()));
	scoreLabels.at(player)->setText(pts);
}

void MainWindow::addScore(int score, Player *p) {
	for (int i = 0; i < players.size(); ++i) {
		if (players.at(i) == p) {
			addScore(score, i);
			return;
		}
	}
}

void MainWindow::addMerchandise(int barrels, int barley, int spools) {
	players.at(turn)->addMerchandise(barrels, barley, spools);
	for (int i = 0; i < barrels; ++i) {
		QLabel *l = new QLabel(window);
		l->setPixmap(*barrelImg);
		merchandiseLayouts.at(turn)->addWidget(l);
		merchandiseLabels.at(turn)->push_back(l);
		l->setStyleSheet(tr("border: 0px"));
	}
	for (int i = 0; i < barley; ++i) {
		QLabel *l = new QLabel(window);
		l->setPixmap(*barleyImg);
		merchandiseLayouts.at(turn)->addWidget(l);
		merchandiseLabels.at(turn)->push_back(l);
		l->setStyleSheet(tr("border: 0px"));
	}
	for (int i = 0; i < spools; ++i) {
		QLabel *l = new QLabel(window);
		l->setPixmap(*clothImg);
		merchandiseLayouts.at(turn)->addWidget(l);
		merchandiseLabels.at(turn)->push_back(l);
		l->setStyleSheet(tr("border: 0px"));
	}
}

void MainWindow::setFollowerType(FollowerType ft) {
	followerType = ft;
}

void MainWindow::countFinalScore() {
	// Make sure that the hasPigs() function returns the correct value for each follower
	checkFarmerPigStatus();
	// 1st edition rules are quite different with regards to farms. Deal with that here.
	if (rules == 1) {
		vector<int> farmScores = table->scoreFirstEditionFarms();
		for (int i = 0; i < players.size(); ++i) {
			addScore(farmScores.at(i), players.at(i));
			vector<Follower*> f = players.at(i)->getAllFollowers();
			for (int j = 0; j < f.size(); ++j) {
				if (!f.at(j)->isPlayable() && f.at(j)->getPlacement()->getTerrain() == field && f.at(j)->getPlacement()->getTerrainAddon() != monastery) {
					restoreFollowerToPlayer(f.at(j));
				}
			}
		}
	}
	// Go through the features with followers in them and count the score
	for (int i = 0; i < players.size(); ++i) {
		vector<Follower*> followers = players.at(i)->getAllFollowers();
		for (int j = 0; j < followers.size(); ++j) {
			if (followers.at(j)->isPlayable()) {
				continue;
			}
			Subtile *sub = followers.at(j)->getPlacement();
			set<Follower*> followersInFeature = table->getAllFollowersInFeature(sub);
			vector<int> playerWeight;
			playerWeight.assign(players.size(), 0);
			vector<bool> pigOnField;
			pigOnField.assign(players.size(), false);
			vector<bool> playerHasFairy;
			playerHasFairy.assign(players.size(), false);
			for (set<Follower*>::iterator it=followersInFeature.begin(); it!=followersInFeature.end(); ++it) {
				Follower *temp = *it;
				switch(temp->getType()) {
				case big:
					++playerWeight.at(temp->getParent()->getPlayerNumber());
				case normal:
					++playerWeight.at(temp->getParent()->getPlayerNumber());
					if (temp->hasFairy()) {
						playerHasFairy.at(temp->getParent()->getPlayerNumber()) = true;
					}
					break;
				case pig:
					pigOnField.at(temp->getParent()->getPlayerNumber()) = true;
					break;
				default:
					break;
				}
				restoreFollowerToPlayer(temp);
			}
			int max = getMaxValue(playerWeight);
			if (max > 0) {
				for (int k = 0; k < playerWeight.size(); ++k) {
					if (playerWeight.at(k) == max) {
						int score = table->getFeatureScore(sub, rules, k, pigOnField, true);
						addScore(score, k);
						if (playerHasFairy.at(k)) {
							addScore(3, k);
						}
					}
				}
			}
		}
	}
	// Finally add points for merchandise
	vector<int> winePts;
	vector<int> grainPts;
	vector<int> clothPts;
	for (int i = 0; i < players.size(); ++i) {
		winePts.push_back(players.at(i)->getWine());
		grainPts.push_back(players.at(i)->getGrain());
		clothPts.push_back(players.at(i)->getCloth());
	}
	int wineMax = getMaxValue(winePts);
	int grainMax = getMaxValue(grainPts);
	int clothMax = getMaxValue(clothPts);
	for (int i = 0; i < players.size(); ++i) {
		if (wineMax > 0 && players.at(i)->getWine() == wineMax) {
			addScore(10, i);
		}
		if (grainMax > 0 && players.at(i)->getGrain() == grainMax) {
			addScore(10, i);
		}
		if (clothMax > 0 && players.at(i)->getCloth() == clothMax) {
			addScore(10, i);
		}
	}
}

void MainWindow::checkFarmerPigStatus() {
	for (int i = 0; i < players.size(); ++i) {
		vector<Follower*> f = players.at(i)->getAllFollowers();
		for (int j = 0; j < f.size(); ++j) {
			if (!f.at(j)->isPlayable() && f.at(j)->getOccupation() == farmer && table->doesFeatureContainPigs(f.at(j)->getPlacement(), players.at(i))) {
				f.at(j)->setPigs(true);
			}
		}
	}
}

// Keeps track of the positions on the table where one may place tiles
void MainWindow::updateValidPositions(int x, int y, bool first) {
	int h = table->getHeight();
	int w = table->getWidth();
	vector<Tile*> tiles = table->getTiles();
	// Contains both x & y jammed into one int. The 8 least significant
	// bits contain y, the 8 following contain x.
	int bothXY;
	bool north = (y > 0 && tiles.at((y - 1) * w + x) == NULL);
	bool east = (x < w - 1 && tiles.at(y * w + x + 1) == NULL);
	bool south = (y < h - 1 && tiles.at((y + 1) * w + x) == NULL);
	bool west = (x > 0 && tiles.at(y * w + x - 1) == NULL);
	set<int> riverEdge = deck->getTile(drawn)->getRiverEdge(north, east, south, west);
	if (!first) {
		bothXY = (x << 8) + y;
		validPlacePositions.erase(bothXY);
		if (!validRiverPositions.empty()) {
			validRiverPositions.erase(bothXY);
		}
	}
	if (north) {
		bothXY = (x << 8) + y - 1;
		validPlacePositions.insert(bothXY);
		if (riverEdge.find(0) != riverEdge.end()) {
			validRiverPositions.insert(bothXY);
		}
	}
	if (east) {
		bothXY = ((x + 1) << 8) + y;
		validPlacePositions.insert(bothXY);
		if (riverEdge.find(1) != riverEdge.end()) {
			validRiverPositions.insert(bothXY);
		}
	}
	if (south) {
		bothXY = (x << 8) + y + 1;
		validPlacePositions.insert(bothXY);
		if (riverEdge.find(2) != riverEdge.end()) {
			validRiverPositions.insert(bothXY);
		}
	}
	if (west) {
		bothXY = ((x - 1) << 8) + y;
		validPlacePositions.insert(bothXY);
		if (riverEdge.find(3) != riverEdge.end()) {
			validRiverPositions.insert(bothXY);
		}
	}
}

bool MainWindow::isValidPosition(int x, int y) {
	int bothXY = (x << 8) + y;
	if (deck->getTile(drawn)->hasTerrain(river)) {
		set<int>::iterator it = validRiverPositions.find(bothXY);
		return (it != validRiverPositions.end());
	}
	set<int>::iterator it = validPlacePositions.find(bothXY);
	return (it != validPlacePositions.end());
}

bool MainWindow::riverDoesUTurn(int x, int y) {
	Tile *t = deck->getTile(drawn);
	vector<Tile*> neighbors = table->getNeighbors(x, y);
	for (int i = 0; i < neighbors.size(); ++i) {
		if (neighbors.at(i) == NULL) {
			continue;
		}
		// Get the river edges of the current tile that is NOT the one we're
		// matching the old tile against.
		set<int> current = t->getRiverEdge(i != 0, i != 1, i != 2, i != 3);		
		// Get the river edges of the neighboring tile that is NOT the one we're
		// matching this tile against.
		set<int> old = neighbors.at(i)->getRiverEdge(i != 2, i != 3, i != 0, i != 1);
		for (set<int>::iterator it = current.begin(); it != current.end(); ++it) {
			int currentEdge = *it;
			if (old.find(currentEdge) != old.end()) {
				return true;
			}
		}
	}
	return false;
}

bool MainWindow::canPlaceFairy() {
	vector<Follower*> followers = players.at(turn)->getAllFollowers();
	for (int i = 0; i < followers.size(); ++i) {
		if (!followers.at(i)->isPlayable()) {
			if (fairyPiece->getSubtile() == NULL) {
				return true;
			}
			if (followers.at(i)->getPlacement()->getParent() != fairyPiece->getSubtile()->getParent()) {
				return true;
			}
		}
	}
	return false;
}

vector<Follower*> MainWindow::followersToEvict() {
	vector<Follower*> f;
	if (!deck->getTile(drawn)->hasTerrainAddon(princess)) {
		return f;
	}
	vector<Subtile*> s = deck->getTile(drawn)->getTerrainAddon(princess);
	set<Follower*> fs = table->getAllFollowersInFeature(s.at(0));
	for (set<Follower*>::iterator it = fs.begin(); it != fs.end(); ++it) {
		Follower *temp = *it;
		if (temp->getType() == normal || temp->getType() == big) {
			f.push_back(temp);
		}
	}
	return f;
}

Follower* MainWindow::haveUserPickFollower(vector<Follower*> fs, bool addPlayer) {
	Follower *f;
	QStringList strLst;
	for (int i = 0; i < fs.size(); ++i) {
		QString temp = QString::number(i);
		temp.append(": ");
		if (addPlayer) {
			temp.append("Player ");
			temp.append(QString::number(fs.at(i)->getParent()->getPlayerNumber() + 1));
			temp.append(" ");
		}
		switch (fs.at(i)->getOccupation()) {
		case knight:
			temp.append("Knight");
			break;
		case robber:
			temp.append("Robber");
			break;
		case farmer:
			temp.append("Farmer");
			break;
		case monk:
			temp.append("Monk");
			break;
		default:
			temp.append("Follower");
			break;
		}
		temp.append(" in the ");
		switch (fs.at(i)->getPlacement()->getParent()->getTruePosition(fs.at(i)->getPlacement()->getPosition())) {
		case 0:
		case 1:
		case 2:
			temp.append("north");
			break;
		case 3:
		case 4:
		case 5:
			temp.append("east");
			break;
		case 6:
		case 7:
		case 8:
			temp.append("south");
			break;
		case 9:
		case 10:
		case 11:
			temp.append("west");
			break;
		default:
			temp.append("middle");
			break;
		}
		strLst << temp;
	}
	bool ok;
	QString follower;
	do {
		follower = QInputDialog::getItem(this, tr("Pick follower"), tr("The tile contains more than one follower. Please specify."), strLst, 0, false, &ok);
	} while (!ok || follower.isEmpty());
	f = fs.at(follower.split(":")[0].toInt());
	return f;
}
