#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QThread>

class QTcpSocket;

class SocketServer : public QThread
{
	Q_OBJECT

public:
	explicit SocketServer(QTcpSocket *socket, int id, QObject *parent = 0);

private slots:
	void handler_readyRead(void);
	void handler_disconnected(void);

signals:
	void closed(int id);

private:
	QTcpSocket *mSocket;
	int mId;

	void run(void);


};

#endif // SOCKETSERVER_H
