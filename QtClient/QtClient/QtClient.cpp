#include "QtClient.h"

QtClient::QtClient(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setFixedSize(QSize(500, 400));
	LoginInfer loginInfer(this);
	connect(&loginInfer, SIGNAL(transName(QString)), this, SLOT(recvName(QString)));
	loginInfer.exec();
	ui.commonArea->setReadOnly(true);
	NetModule::Instance()->init();
	NetModule::Instance()->netConnect();
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(sendMsg()));
	connect(this, SIGNAL(transMsg(QString)), this, SLOT(recvMsg(QString)));
	startRecvMsg();
}
void QtClient::startRecvMsg()
{
	recvMsgThread = std::thread([&]() {
		while (true) {
			QString newMsg = QString::fromLocal8Bit(NetModule::Instance()->recvMsg().c_str());
			if (newMsg == "") break;
			emit transMsg(newMsg);
		}
	});
}
void QtClient::recvName(QString name)
{
	user.name = name;
}
void QtClient::sendMsg()
{
	QString str = user.name + ":" + ui.inputArea->toPlainText();
	NetModule::Instance()->sendMsg(str.toLocal8Bit().toStdString());
	ui.inputArea->clear();
}
void QtClient::recvMsg(QString msg)
{
	ui.commonArea->setPlainText(ui.commonArea->toPlainText() + msg + "\n");
}
void QtClient::closeEvent(QCloseEvent* e)
{
	recvMsgThread.detach();
}