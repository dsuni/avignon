#include "followerplacer.h"

FollowerPlacer::FollowerPlacer(QWidget *parent, bool portal) {
	image = NULL;
	other = NULL;
	buttonLayout = NULL;
	layout = new QVBoxLayout(); // Deleted in destructor
	label = new TableLabel();
	combo = new QComboBox();
	cancel = new QPushButton(tr("Cancel"));
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
	connect(cancel, SIGNAL(clicked()), this, SLOT(changedMyMind()));
	this->setLayout(layout);
	layout->addWidget(combo);
	layout->addWidget(label);
	if (portal) {
		other = new QPushButton(tr("Pick other tile"));
		connect(other, SIGNAL(clicked()), this, SLOT(changedMyMind()));
		buttonLayout = new QHBoxLayout();
		layout->addLayout(buttonLayout);
		buttonLayout->addWidget(cancel);
		buttonLayout->addWidget(other);
	}
	else {
		layout->addWidget(cancel);
	}
}

FollowerPlacer::~FollowerPlacer() {
	delete cancel;
	cancel = NULL;
	delete other;
	other = NULL;
	delete buttonLayout;
	buttonLayout = NULL;
	delete label;
	label = NULL;
	delete combo;
	combo = NULL;
	delete layout;
	layout = NULL;
}

void FollowerPlacer::setImage(QPixmap *p) {
	image = p;
	label->setPixmap(*image);
	this->show();
}

void FollowerPlacer::setFollowers(vector<Follower*> f) {
	map<FollowerType, int> fCounter;
	for (int i = followerTypeFirst; i <= followerTypeLast; ++i) {
		fCounter[(FollowerType)i] = 0;
	}
	for (int i = 0; i < f.size(); ++i) {
		if (f.at(i)->isPlayable()) {
			++fCounter[f.at(i)->getType()];
		}
	}
	bool first = true;
	for (int i = followerTypeFirst; i <= followerTypeLast; ++i) {
		if (fCounter[(FollowerType)i] > 0) {
			QString str(followerTypeMap()[(FollowerType)i].c_str());
			str.append(tr(" ("));
			str.append(QString::number(fCounter[(FollowerType)i]));
			str.append(tr(")"));
			combo->addItem(str);
			if (first) {
				emit followerTypeSelected((FollowerType)i);
				first = false;
			}
		}
	}
}

TableLabel* FollowerPlacer::getLabel() {
	return label;
}

void FollowerPlacer::comboChanged(int index) {
	QString str = combo->currentText().split(" ")[0];
	if (str == "Big") {
		emit followerTypeSelected(big);
	}
	else if (str == "Builder") {
		emit followerTypeSelected(builder);
	}
	else if (str == "Pig") {
		emit followerTypeSelected(pig);
	}
	else {
		emit followerTypeSelected(normal);
	}
}

void FollowerPlacer::changedMyMind() {
	QPushButton *button = (QPushButton*)QObject::sender();
	if (button == other) {
		emit(keepPortalActive());
	}
	this->close();
}
