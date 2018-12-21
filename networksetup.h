#ifndef NETWORKSETUP_H
#define NETWORKSETUP_H
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QIntValidator>
#include <QDnsLookup>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QDebug>
#include "server.h"
#include "client.h"
#include "gamecreation.h"
#include "mainwindow.h"

using namespace std;

class Server;

class NetworkSetup : public QMainWindow {
	Q_OBJECT

private:
	QVBoxLayout *layout;
	QHBoxLayout *addressLayout;
	QWidget *window;
	QButtonGroup *buttons;
	QRadioButton *local;
	QRadioButton *server;
	QRadioButton *client;
	QLineEdit *address;
	QLineEdit *port;
	QTextEdit *messages;
	QLabel *addressLabel;
	QLabel *portLabel;
	QPushButton *start;
	QIntValidator *val;
	QDnsLookup *dns;
	Client *cli;
	Server *serv;
	int connectedClients;
	bool isIpAddress(QString str);
	void connectToServer();

public:
	NetworkSetup(QWidget *parent = 0);
	~NetworkSetup();
	void clientConnected(QString ip);

private slots:
	void radioButtonClicked(int id);
	void nextClicked();
	void handleDNS();
};
#endif
