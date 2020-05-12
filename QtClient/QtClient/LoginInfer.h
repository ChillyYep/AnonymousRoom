#pragma once

#include <QDialog>
#include <qmessagebox.h>
#include "ui_LoginInfer.h"
#pragma execution_character_set("utf-8")

class LoginInfer : public QDialog
{
	Q_OBJECT

public:
	LoginInfer(QWidget *parent = Q_NULLPTR);
	~LoginInfer();

private:
	Ui::LoginInfer ui;
signals:
	void transName(QString name);
private slots:
	void login();
};
