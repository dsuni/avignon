#include "gamecreation.h"

GameCreation::GameCreation(QWidget *parent) {
	window = new QWidget(); // Deleted in destructor
	layout = new QVBoxLayout(window);
	deckLayout = new QGridLayout(window);
	table = new QHBoxLayout(window);
	widthBox = new QSpinBox(window);
	heightBox = new QSpinBox(window);
	playerBox = new QSpinBox(window);
	widthBox->setRange(MIN_TABLE_SIZE, MAX_TABLE_SIZE);
	heightBox->setRange(MIN_TABLE_SIZE, MAX_TABLE_SIZE);
	playerBox->setRange(2, 6);
	widthBox->setValue(DEFAULT_TABLE_SIZE);
	heightBox->setValue(DEFAULT_TABLE_SIZE);
	playerBox->setValue(2);
	widthLabel = new QLabel(tr("Table width:"), window);
	heightLabel = new QLabel(tr("Table height:"), window);
	playerLabel = new QLabel(tr("Players:"), window);
	deckLabel = new QLabel(tr("Decks"), window);
	rulesLabel = new QLabel(tr("Rules"), window);
	QFont font = deckLabel->font();
	font.setPointSize(18);
	font.setBold(true);
	deckLabel->setFont(font);
	rulesLabel->setFont(font);
	rulesGroup = new QButtonGroup(window);
	edition1 = new QRadioButton(tr("1st edition scoring"), window);
	edition2 = new QRadioButton(tr("2nd edition scoring"), window);
	edition3 = new QRadioButton(tr("3rd edition scoring"), window);
	rulesGroup->addButton(edition1);
	rulesGroup->addButton(edition2);
	rulesGroup->addButton(edition3);
	edition1->setChecked(true);
	QDir dir("decks");
	QStringList filter;
	filter << "*.xml";
	dir.setNameFilters(filter);
	xmls = new QStringList(); // Deleted in destructor
	*xmls = dir.entryList();
	for (int i = 0; i < xmls->size(); ++i) {
		QString temp = xmls->at(i).split(".")[0].replace('_', ' ');
		deckBox.push_back(new QCheckBox(temp, window));
		rulesBox.push_back(new QCheckBox(window));
		if (xmls->at(i) == "core.xml") {
			deckBox[i]->setChecked(true);
			rulesBox[i]->setChecked(true);
			rulesBox[i]->setEnabled(false);
		}
		else if (xmls->at(i) == "river.xml" || xmls->at(i) == "river_II.xml") {
			rulesBox[i]->setEnabled(false);
		}
		connect(deckBox.at(i), SIGNAL(clicked()), this, SLOT(boxClicked()));
	}
	start = new QPushButton(tr("Start game"), window);
	connect(start, SIGNAL(clicked()), this, SLOT(startButton()));

	window->setLayout(layout);
	setCentralWidget(window);
	table->addWidget(widthLabel);
	table->addWidget(widthBox);
	table->addWidget(heightLabel);
	table->addWidget(heightBox);
	table->addWidget(playerLabel);
	table->addWidget(playerBox);
	layout->addLayout(table);
	layout->addWidget(edition1);
	layout->addWidget(edition2);
	layout->addWidget(edition3);
	deckLayout->addWidget(deckLabel, 0, 0);
	deckLayout->addWidget(rulesLabel, 0, 1);
	layout->addLayout(deckLayout);
	for (int i = 0; i < deckBox.size(); ++i) {
		deckLayout->addWidget(deckBox.at(i), i + 1, 0);
		deckLayout->addWidget(rulesBox.at(i), i + 1, 1);
	}
	layout->addWidget(start);
}

GameCreation::~GameCreation() {
	delete window;
	window = NULL;
	delete xmls;
	xmls = NULL;
}

void GameCreation::startButton() {
	usedDecks = new QStringList(); // If handover to Mainwindow successful, deleted by MW,
	usedRules = new QStringList(); // if failed, deleted at end of function.
	for (int i = 0; i < xmls->size(); ++i) {
		if (deckBox.at(i)->isChecked()) {
			*usedDecks << xmls->at(i);
		}
		if (rulesBox.at(i)->isChecked()) {
			*usedRules << xmls->at(i);
		}
	}
	if (usedDecks->size() > 0) {
		emit startGame(widthBox->value(), heightBox->value(), playerBox->value(), rulesGroup->checkedId(), usedDecks, usedRules);
		this->close();
	}
	else {
		delete usedDecks;
		delete usedRules;
	}
}

void GameCreation::boxClicked() {
	QCheckBox *box = (QCheckBox*)QObject::sender();
	for (int i = 0; i < deckBox.size(); ++i) {
		if (box == deckBox.at(i)) {
			if (deckBox.at(i)->text() == "core") {
				return;
			}
			rulesBox.at(i)->setChecked(deckBox.at(i)->isChecked());
		}
	}
}
