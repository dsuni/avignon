#ifndef CLIENT_H
#define CLIENT_H
#include <QThread>
#include <QTcpSocket>
#include <QString>
#include <QDebug>

using namespace std;

class Client : public QThread {
	Q_OBJECT

private:
	QTcpSocket *socket;
	QString address;
	int port;

public:
	Client(QString a, int p, QObject *parent = 0);
	void run();

public slots:
	void readyRead();
	void disconnected();

signals:
	void error(QTcpSocket::SocketError socketerror);
};
#endif
