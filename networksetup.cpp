#include "networksetup.h"

NetworkSetup::NetworkSetup(QWidget *parent) {
	connectedClients = 0;
	window = new QWidget(); // Deleted in destructor
	val = new QIntValidator(1024, 65535, this); // Deleted in destructor
	layout = new QVBoxLayout(window);
	buttons = new QButtonGroup(window);
	local = new QRadioButton(tr("Start a local game"), window);
	server = new QRadioButton(tr("Start a game server"), window);
	client = new QRadioButton(tr("Connect to a game server"), window);
	buttons->addButton(local);
	buttons->addButton(server);
	buttons->addButton(client);
	connect(buttons, SIGNAL(buttonClicked(int)), this, SLOT(radioButtonClicked(int)));
	local->setChecked(true);
	addressLayout = new QHBoxLayout(window);
	address = new QLineEdit(window);
	address->setFixedWidth(220);
	address->setEnabled(false);
	port = new QLineEdit(tr("9595"), window);
	port->setMaxLength(5);
	port->setValidator(val);
	port->setFixedWidth(60);
	port->setEnabled(false);
	addressLabel = new QLabel(tr("Address:"), window);
	portLabel = new QLabel(tr("Port:"), window);
	start = new QPushButton(tr("Next"), window);
	connect(start, SIGNAL(clicked()), this, SLOT(nextClicked()));
	window->setLayout(layout);
	setCentralWidget(window);
	layout->addWidget(local);
	layout->addWidget(server);
	layout->addWidget(client);
	layout->addLayout(addressLayout);
	messages = new QTextEdit(window);
	messages->setEnabled(false);
	messages->setVisible(false);
	messages->setPlaceholderText(tr("Waiting for connections..."));
	layout->addWidget(messages);
	addressLayout->addWidget(addressLabel);
	addressLayout->addWidget(address);
	addressLayout->addWidget(portLabel);
	addressLayout->addWidget(port);
	layout->addWidget(start);
}

NetworkSetup::~NetworkSetup() {
	delete val;
	val = NULL;
	delete window;
	window = NULL;
}

void NetworkSetup::clientConnected(QString ip) {
	QString temp = QString("Client connected from address ");
	temp.append(ip);
	messages->append(temp);
	++connectedClients;
	if (connectedClients == 5) {
		serv->setAcceptConnections(false);
		messages->append(tr("Player slots full. Will accept no more connections."));
	}
	start->setEnabled(true);
}

void NetworkSetup::radioButtonClicked(int id) {
	switch (id) {
	case -2:
		address->setEnabled(false);
		port->setEnabled(false);
		break;
	case -3:
		address->setEnabled(false);
		port->setEnabled(true);
		break;
	case -4:
		address->setEnabled(true);
		port->setEnabled(true);
		break;
	default:
		break;
	}
}

void NetworkSetup::nextClicked() {
	if (connectedClients > 0) {
		serv->setAcceptConnections(false);
		MainWindow *mw = new MainWindow();
		GameCreation *gc = new GameCreation();
		gc->show();
		QObject::connect(gc, SIGNAL(startGame(int, int, int, int, QStringList*, QStringList*)), mw, SLOT(start(int, int, int, int, QStringList*, QStringList*)));
		this->hide();
	}
	else if (server->isChecked()) {
		serv = new Server(port->text().toInt(), this);
		serv->startServer();
		start->setEnabled(false);
		messages->setVisible(true);
	}
	else if (client->isChecked()) {
		if (isIpAddress(address->text())) {
			connectToServer();
		}
		else {
			start->setEnabled(false);
			dns = new QDnsLookup(this);
			connect(dns, SIGNAL(finished()),this, SLOT(handleDNS()));
			dns->setType(QDnsLookup::A);
			dns->setName(address->text());
			dns->lookup();
			return;
		}
	}
}

void NetworkSetup::handleDNS() {
	if (dns->error() != QDnsLookup::NoError) {
		qDebug() << dns->error();
		QMessageBox::critical(this, "DNS lookup failed", "Can not find any such server.", QMessageBox::Ok);
		delete dns;
		dns = NULL;
		start->setEnabled(true);
		return;
	}
	connectToServer();
}

bool NetworkSetup::isIpAddress(QString str) {
	QStringList strLst = str.split(".");
	if (strLst.size() != 4) {
		return false;
	}
	int num;
	bool ok;
	for (int i = 0; i < 4; ++i) {
		num = strLst[i].toInt(&ok);
		if (!ok || num < 0 || num > 255) {
			return false;
		}
	}
	return true;
}

void NetworkSetup::connectToServer() {
	cli = new Client(address->text(), port->text().toInt());
	connect(cli, SIGNAL(finished()), cli, SLOT(deleteLater()));
	cli->start();
}
