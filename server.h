#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QString>
#include <map>
#include "serverthread.h"
#include "networksetup.h"

using namespace std;

class NetworkSetup;

class Server : public QTcpServer {
	Q_OBJECT

private:
	int port;
	bool acceptConnections;
  map<qintptr, ServerThread*> threads;
	NetworkSetup *setup;

protected:
	void incomingConnection(qintptr socketDescriptor);

public:
	Server(int p, NetworkSetup *s, QObject *parent = 0);
	void startServer();
	void setAcceptConnections(bool conn);

public slots:
	void messageParent(QString ip);
};
#endif
