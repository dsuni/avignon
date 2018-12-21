#include "serverthread.h"

ServerThread::ServerThread(qintptr id, QObject *parent) : socketDescriptor(id), QThread(parent) {}

void ServerThread::run() {
	socket = new QTcpSocket();
	if (!socket->setSocketDescriptor(this->socketDescriptor)) {
		emit error(socket->error());
		return;
	}
	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
	connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	qDebug() << socketDescriptor << " Client connected";
	socket->write("Hi\n");
	emit setupDone(socket->peerAddress().toString());
	exec();
}

void ServerThread::readyRead() {
	QByteArray data = socket->readAll();
	qDebug() << socketDescriptor << " Data in: " << data;
	socket->write(data);
}

void ServerThread::disconnected() {
	qDebug() << socketDescriptor << " Disconnected";
	socket->deleteLater();
	exit(0);
}
