#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QDebug>

using namespace std;

class ServerThread : public QThread {
	Q_OBJECT

private:
	QTcpSocket *socket;
	qintptr socketDescriptor;

public:
	explicit ServerThread(qintptr id, QObject *parent = 0);
	void run();

public slots:
	void readyRead();
	void disconnected();

signals:
	void error(QTcpSocket::SocketError socketerror);
	void setupDone(QString ip);
};
#endif
