#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <SocketServer.h>

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mServer = new QTcpServer(this);
	connect(mServer, &QTcpServer::newConnection, this, &MainWindow::handler_newConnection);
	mServer->listen(QHostAddress::Any, 8080);

	mServerCount = 0;
	for(int i=0;i<MAX_SOCKET_SERVER;i++)
		mSocketServer[i] = 0;
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::handler_newConnection(void)
{
	QTcpSocket *socket;

	while(mServer->hasPendingConnections())
	{
		if(mServerCount >= MAX_SOCKET_SERVER)
			goto error_handle;

		socket = mServer->nextPendingConnection();
		for(int i=0;i<MAX_SOCKET_SERVER;i++)
		{
			if(mSocketServer[i] == 0)
			{
				mSocketServer[i] = new SocketServer(socket, i);
				connect(mSocketServer[i], &SocketServer::closed, this, &MainWindow::handler_closed);
				mServerCount++;
				break;
			}
		}
	}

	return;

error_handle:
	socket = mServer->nextPendingConnection();
	socket->close();
	return;
}

void MainWindow::handler_closed(int id)
{
	delete mSocketServer[id];
	mServerCount--;
}
