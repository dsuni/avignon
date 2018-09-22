#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QTransform>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <vector>
#include <string>
#include "deck.h"
#include "table.h"
#include "player.h"
#include "follower.h"
#include "followerplacer.h"
#include "occupationmapper.h"
#include "piece.h"
#include "dragonmover.h"

using namespace std;

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	int tileSizes[6] = {25, 50, 100, 200, 400, 800};
	int zoom;
	int screenSize;
	int drawn;
	int turn;
	int rules;
	bool active;
	bool portalActive;
	bool gameOver;
	bool builderRedraw;
	bool builderPlaced;
	bool innsAndCathedrals;
	bool tradersAndBuilders;
	bool princessAndDragon;
	bool volcanoPlayed;
	bool placeFairy;
	QString *baseDir;
	QWidget *window;
	QWidget *tableArea;
	QWidget *rotateButtons;
	QWidget *zoomButtons;
	QScrollArea *scroll;
	QHBoxLayout *mainLayout;
	QHBoxLayout *rotateLayout;
	QHBoxLayout *zoomLayout;
	QGridLayout *tableLayout;
	QVBoxLayout *controlLayout;
	QLabel *currentDraw;
	QLabel *tileCounter;
	QPixmap *backDrawPile;
	QPixmap *backTable;
	QPixmap *barrelImg;
	QPixmap *barleyImg;
	QPixmap *clothImg;
	Table *table;
	Deck *deck;
	QPushButton *draw;
	QPushButton *rotateCCW;
	QPushButton *rotateCW;
	QPushButton *zoomInButton;
	QPushButton *zoomOutButton;
	vector<Player*> players;
	vector<QLabel*> scoreLabels;
	vector<QWidget*> scoreWidgets;
	vector<QVBoxLayout*> scoreLayouts;
	vector<QHBoxLayout*> merchandiseLayouts;
	vector<vector<QLabel*>*> merchandiseLabels;
	set<int> validPlacePositions;
	set<int> validRiverPositions;
	FollowerPlacer *placer;
	FollowerType followerType;
	TableLabel *currentLabel;
	OccupationMapper *occupationMapper;
	Piece *fairyPiece;
	Piece *dragonPiece;
	Tile *placingTile;
	bool legalMovesExist(Tile *t);
	bool riverDoesUTurn(int x, int y);
	void checkFeatureCompletion();
	void advanceTurn();
	void redrawTable();
	int translatePointToSubtile(int x, int y, int size);
	void restoreFollowerToPlayer(Follower *f);
	int getMaxValue(vector<int> nums);
	void addScore(int score, int player);
	void addScore(int score, Player *p);
	void addMerchandise(int barrels, int barley, int spools);
	void countFinalScore();
	void checkFarmerPigStatus();
	void updateValidPositions(int x, int y, bool first = false);
	bool isValidPosition(int x, int y);
	bool canPlaceFairy();
	vector<Follower*> followersToEvict();
	Follower* haveUserPickFollower(vector<Follower*> fs, bool addPlayer);

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void drawTile();
	void selectedTableLabel();
	void rotateClockwise();
	void rotateCounterClockwise();
	void zoomIn();
	void zoomOut();

public slots:
	void start(int x, int y, int p, int r, QStringList *decks, QStringList *rulesList);
	void placeFollower(const QPoint p);
	void setFollowerType(FollowerType ft);
	void keepPortalActive();
};
#endif
