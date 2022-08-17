#include "SocketServer.h"
#include <QTcpSocket>

SocketServer::SocketServer(QTcpSocket *socket, int id, QObject *parent) : QThread(parent)
{
	mSocket = socket;
	mId = id;

	connect(socket, &QTcpSocket::readyRead, this, &SocketServer::handler_readyRead);
	connect(socket, &QTcpSocket::disconnected, this, &SocketServer::handler_disconnected);
}

void SocketServer::run(void)
{

}

void SocketServer::handler_readyRead(void)
{
	char buf[512];

	for(int i=0;i<512;i+=2)
	{
		*(short*)&buf[i] = i/2;
	}

	mSocket->write(buf, 512);

//	mSocket->close();
//	emit closed(mId);
}

void SocketServer::handler_disconnected(void)
{

}

