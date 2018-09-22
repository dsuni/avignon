#ifndef GAMECREATION_H
#define GAMECREATION_H
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFont>
#include <vector>
#include "table.h"
#include "deck.h"

using namespace std;

class GameCreation : public QMainWindow {
	Q_OBJECT

private:
	QVBoxLayout *layout;
	QGridLayout *deckLayout;
	QHBoxLayout *table;
	QWidget *window;
	QLabel *widthLabel;
	QLabel *heightLabel;
	QLabel *playerLabel;
	QLabel *deckLabel;
	QLabel *rulesLabel;
	QSpinBox *widthBox;
	QSpinBox *heightBox;
	QSpinBox *playerBox;
	vector<QCheckBox*> deckBox;
	vector<QCheckBox*> rulesBox;
	QButtonGroup *rulesGroup;
	QRadioButton *edition1;
	QRadioButton *edition2;
	QRadioButton *edition3;
	QPushButton *start;
	QStringList *xmls;
	QStringList *usedDecks;
	QStringList *usedRules;

public:
	GameCreation(QWidget *parent = 0);
	~GameCreation();

private slots:
	void startButton();
	void boxClicked();

signals:
	void startGame(int x, int y, int p, int r, QStringList *decks, QStringList *rules);
};
#endif
