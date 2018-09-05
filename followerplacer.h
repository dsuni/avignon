#ifndef FOLLOWERPLACER_H
#define FOLLOWERPLACER_H
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QPixmap>
#include "tablelabel.h"
#include "follower.h"

class FollowerPlacer : public QDialog {
	Q_OBJECT

private:
	TableLabel *label;
	QVBoxLayout *layout;
	QHBoxLayout *buttonLayout;
	QComboBox *combo;
	QPixmap *image;
	QPushButton *cancel;
	QPushButton *other;

public:
	FollowerPlacer(QWidget *parent, bool portal);
	~FollowerPlacer();
	void setImage(QPixmap *p);
	void setFollowers(vector<Follower*> f);
	TableLabel* getLabel();

signals:
	void followerTypeSelected(FollowerType);
	void keepPortalActive();

private slots:
	void comboChanged(int index);
	void changedMyMind();
};
#endif
