#include "server.h"

Server::Server(int p, NetworkSetup *s, QObject *parent) : port(p), setup(s), QTcpServer(parent) {
	acceptConnections = true;
}

void Server::startServer() {
	if(!this->listen(QHostAddress::Any, port)) {
		qDebug() << "Could not start server";
	}
	else {
		qDebug() << "Listening to port " << port << "...";
	}
}

void Server::setAcceptConnections(bool conn) {
	acceptConnections = conn;
}

void Server::incomingConnection(qintptr socketDescriptor) {
	if (!acceptConnections) {
		return;
	}
	qDebug() << socketDescriptor << " Connecting...";
	ServerThread *thread = new ServerThread(socketDescriptor, this);
	threads[socketDescriptor] = thread;
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(setupDone(QString)), this, SLOT(messageParent(QString)));
	thread->start();
}

void Server::messageParent(QString ip) {
	setup->clientConnected(ip);
}
