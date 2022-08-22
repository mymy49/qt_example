#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define MAX_SOCKET_SERVER	20

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class QTcpServer;
class SocketServer;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

	bool isFileLoaded(void);
	unsigned int getPacketCount(void);

private slots:
	void handler_newConnection(void);

	void handler_closed(int id);

	void on_pushButton_clicked();

private:
	Ui::MainWindow *ui;
	QTcpServer *mServer;
	SocketServer *mSocketServer[MAX_SOCKET_SERVER];

	unsigned char *mBinFile;
	QString mFileName;
	unsigned int mFileSize, mPacketCount;
	bool mFileLoadFlag;

	int mServerCount;
};
#endif // MAINWINDOW_H
