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

private slots:
	void handler_newConnection(void);

	void handler_closed(int id);

private:
	Ui::MainWindow *ui;
	QTcpServer *mServer;
	SocketServer *mSocketServer[MAX_SOCKET_SERVER];
	int mServerCount;
};
#endif // MAINWINDOW_H
