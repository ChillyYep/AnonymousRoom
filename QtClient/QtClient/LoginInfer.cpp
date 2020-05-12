#include "LoginInfer.h"

LoginInfer::LoginInfer(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.loginBtn, SIGNAL(clicked()), this, SLOT(login()));
}
void LoginInfer::login()
{
	QString& name = ui.nameInput->text();
	if (name == "")
	{
		QMessageBox::warning(this, tr("����"), tr("�ǳƲ���Ϊ�գ�"));
		return;
	}
	emit transName(name);
	close();
}

LoginInfer::~LoginInfer()
{
}
