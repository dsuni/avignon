#include "client.h"

Client::Client(QString a, int p, QObject *parent) : address(a), port(p), QThread(parent) {}

void Client::run() {
	socket = new QTcpSocket();
	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	socket->connectToHost(address, port);
	qDebug() << socket->error() << socket->state();
	exec();
}

void Client::readyRead() {
	QByteArray data = socket->readAll();
	qDebug() << " Data in: " << data;
}

void Client::disconnected() {
	qDebug() << " Disconnected";
	socket->deleteLater();
	exit(0);
}
