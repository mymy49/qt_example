#include "SocketServer.h"
#include <QTcpSocket>
#include <mainwindow.h>
#include <string.h>

#define STX		0x02
#define ECHO	0x0E
#define ETX		0x03

SocketServer::SocketServer(QTcpSocket *socket, int id, MainWindow *parent) : QThread((QObject*)parent)
{
	mSocket = socket;
	mId = id;
	mRcvData = new unsigned char[512];
	mState = 0;
	mParent = parent;

	connect(socket, &QTcpSocket::readyRead, this, &SocketServer::handler_readyRead);
	connect(socket, &QTcpSocket::disconnected, this, &SocketServer::handler_disconnected);
}

void SocketServer::run(void)
{
	delete mRcvData;
}

void SocketServer::handler_readyRead(void)
{
	int len;
	char *rcvData;
	unsigned char data;

	mRcvBuf = mSocket->readAll();
	len = mRcvBuf.length();
	rcvData = mRcvBuf.data();

	for(int i=0;i<len;i++)
	{
		data = rcvData[i];
		switch(mState)
		{
		case 0 : // 0x02 확인
			if(data == STX)
				mState = 1;
			mRcvHeader.stx = 0x02;
			break;

		case 1 : // 명령 수신
			mRcvHeader.message = data;
			mState = 2;
			break;

		case 2 : // size 첫번째 수신
			mRcvHeader.size = data;
			mState = 3;
			break;

		case 3 : // size 두번째 수신
			mRcvHeader.size |= (unsigned short)data << 8;
			mState = 4;
			break;

		case 4 : // index 첫번째 수신
			mRcvHeader.index = data;
			mState = 5;
			break;

		case 5 : // size 두번째 수신
			mRcvHeader.index |= (unsigned short)data << 8;
			mState = 6;
			break;

		case 6 : // 0x0E 확인
			if(data == ECHO)
			{
				if(mRcvHeader.size > 0)
				{
					mState = 7;
					mRcvDataCount = 0;
				}
				else
					mState = 8;
				mRcvHeader.echo = 0x0E;
			}
			else
				mState = 0;
			break;

		case 7 : // 데이터 수신
			mRcvData[mRcvDataCount++] = data;

			if(mRcvDataCount >= mRcvHeader.size)
				mState = 8;
			break;

		case 8 : // ETX 확인
			if(data == ETX)
				mState = 9;
			else
				mState = 0;
			break;

		case 9 : // 체크섬 확인
			if(data == mRcvChksum)
			{
				handleMessage();
			}
			mState = 0;
			break;
		}

		if(mState == 1)
			mRcvChksum = 0x02;
		else
			mRcvChksum ^= data;
	}
}

void SocketServer::respondMessage(unsigned char message, unsigned char *data, unsigned short size)
{
	ProtocolHeader header;
	ProtocolTail tail;

	unsigned char chksum = 0, *buf;
	header.message = message;
	header.index = mRcvHeader.index;
	header.size = size;

	buf = (unsigned char*)&header;
	for(unsigned int i=0;i<sizeof(header);i++)
		chksum ^= buf[i];

	for(int i=0;i<size;i++)
		chksum ^= data[i];

	chksum ^= ETX;
	tail.chksum = chksum;

	mSocket->write((const char*)&header, sizeof(header));
	if(size)
		mSocket->write((const char*)data, size);
	mSocket->write((const char*)&tail, sizeof(tail));
}

void SocketServer::handleMessage(void)
{
	unsigned char sendBuf[512], *firmware;
	int packet;

	switch(mRcvHeader.message)
	{
	case MSG_HOW_ARE_YOU :
		respondMessage(MSG_I_AM_FINE, 0, 0);
		break;

	case MSG_HAVE_YOU_NEW_FIRMWARE :
		if(mParent->isFileLoaded())
			respondMessage(MSG_YES_I_HAVE, 0, 0);
		else
			respondMessage(MSG_NO_I_HAVE_NOT, 0, 0);
		break;

	case MSG_GIVE_ME_TOTAL_PACKET :
		packet = mParent->getPacketCount();
		*(unsigned int*)&sendBuf[0] = packet;
		respondMessage(MSG_IT_IS_TOTAL_PACKET, sendBuf, 4);
		break;

	case MSG_GIVE_ME_FIRMWARE_PACKET :
		packet = *(unsigned int*)&mRcvData[0];
		firmware = mParent->getFirmwareBinary();
		*(unsigned int*)&sendBuf[0] = packet;
		memcpy(&sendBuf[4], &firmware[packet*256], 256);
		respondMessage(MSG_IT_IS_FIRMWARE_PACKET, sendBuf, 256+4);
		break;

	default :
		respondMessage(MSG_I_DON_T_KNOW, 0, 0);
		break;
	}
}

void SocketServer::handler_disconnected(void)
{
	emit closed(mId);
}

