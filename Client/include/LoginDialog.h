#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
	Q_OBJECT

public:
	explicit LoginDialog(QWidget* parent = nullptr);
	~LoginDialog();

private slots:
	void on_button_login_clicked();

private:
	void initUi();
	void setShadow();
	// 设置 UI 错误提示
	void setErrorHint(bool isError, const QString& hint);

private:
	Ui::LoginDialog* ui;
};

#endif // LOGINDIALOG_H
