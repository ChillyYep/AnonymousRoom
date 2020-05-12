#pragma once

#include <QtWidgets/QMainWindow>
#include <qevent.h>
#include "NetModule.h"
#include "LoginInfer.h"
#include "ui_QtClient.h"
#pragma execution_character_set("utf-8")
struct User {
	QString name;
};
class QtClient : public QMainWindow
{
	Q_OBJECT

public:
	QtClient(QWidget *parent = Q_NULLPTR);
protected:
	void closeEvent(QCloseEvent* e);
private:
	Ui::QtClientClass ui;
	User user;
	void startRecvMsg();
	std::thread recvMsgThread;
private slots:
	void sendMsg();
	void recvMsg(QString msg);
	void recvName(QString name);
signals:
	void transMsg(QString msg);
};
