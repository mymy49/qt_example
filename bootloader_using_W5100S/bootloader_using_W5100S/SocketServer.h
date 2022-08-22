#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QThread>
#include <QByteArray>

class QTcpSocket;
class MainWindow;

class SocketServer : public QThread
{
	Q_OBJECT

public:
	explicit SocketServer(QTcpSocket *socket, int id, MainWindow *parent);

private slots:
	void handler_readyRead(void);
	void handler_disconnected(void);

signals:
	void closed(int id);

private:
	enum
	{
		MSG_HOW_ARE_YOU = 0,
		MSG_I_AM_FINE = 1,
		MSG_HAVE_YOU_NEW_FIRMWARE = 2,
		MSG_NO_I_HAVE_NOT = 3,
		MSG_YES_I_HAVE = 4,
		MSG_I_DON_T_KNOW = 5,
		MSG_GIVE_ME_TOTAL_PACKET = 6,
		MSG_IT_IS_TOTAL_PACKET = 7,
		MSG_GIVE_ME_FIRMWARE_PACKET = 8,
		MSG_IT_IS_FIRMWARE_PACKET = 9,
	};

	struct ProtocolHeader
	{
		unsigned char stx = 0x02;
		unsigned char message;
		unsigned short size;
		unsigned short index;
		unsigned char echo = 0x0E;
	}__attribute__((packed));

	struct ProtocolTail
	{
		unsigned char etx = 0x03;
		unsigned char chksum;
	}__attribute__((packed));

	QTcpSocket *mSocket;
	int mId;
	QByteArray mRcvBuf;
	int mState, mRcvSize, mRcvIndex, mRcvDataCount;
	unsigned char mRcvChksum;
	unsigned char *mRcvData;
	ProtocolHeader mRcvHeader;
	ProtocolTail mRcvTail;
	MainWindow *mParent;

	void run(void);
	void handleMessage(void);
	void respondMessage(unsigned char message, unsigned char *data, unsigned short size);
};

#endif // SOCKETSERVER_H
