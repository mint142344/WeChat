#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class LoadingDialog;

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
	LoadingDialog* m_loading_dialog;
};

#endif // LOGINDIALOG_H
