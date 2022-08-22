#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <SocketServer.h>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mServer = new QTcpServer(this);
	connect(mServer, &QTcpServer::newConnection, this, &MainWindow::handler_newConnection);
	mServer->listen(QHostAddress::Any, 8080);

	mServerCount = 0;
	for(int i=0;i<MAX_SOCKET_SERVER;i++)
		mSocketServer[i] = 0;

	mBinFile = new unsigned char[448 * 1024];
	mFileLoadFlag = false;
}

MainWindow::~MainWindow()
{
	delete ui;
	delete mBinFile;
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
				mSocketServer[i] = new SocketServer(socket, i, this);
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

void MainWindow::on_pushButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, "파일 열기", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), "*.bin");

	QFile file(fileName);

	if(file.open(QFile::ReadOnly) == false)
	{
		return;
	}

	mFileSize = file.size();
	if(mFileSize > 448 * 1024)
		return;

	mFileName = fileName;
	ui->lineEdit->setText(mFileName);
	mFileLoadFlag = true;

	file.read((char*)mBinFile, mFileSize);
	file.close();
}

bool MainWindow::isFileLoaded(void)
{
	return mFileLoadFlag;
}

unsigned int MainWindow::getPacketCount(void)
{
	unsigned int packetCount;

	if(mFileLoadFlag)
	{
		packetCount = mFileSize / 256;
		if(mFileSize % 256)
			packetCount += 1;
	}
	else
		packetCount = 0;

	return packetCount;
}


