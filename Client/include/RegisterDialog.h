#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include "Common.h"

#include <QMap>
#include <QDialog>
#include <functional>

namespace Ui {
class RegisterDialog;
}

// 注册对话框
class RegisterDialog : public QDialog {
	Q_OBJECT

public:
	explicit RegisterDialog(QWidget* parent = nullptr);
	~RegisterDialog();

private slots:
	// 邮箱正则检查
	void on_lineEdit_email_editingFinished();
	// 密码正则检查
	void on_lineEdit_password_editingFinished();
	// 获取验证码
	void on_button_get_code_clicked();
	// 确认注册
	void on_button_confirm_clicked();

	void http_request_finished(RequestType request_type, const QJsonObject& json);

private:
	// 注册网络请求回调
	void registerAllCallback();

	// 设置 UI 错误提示
	void setErrorHint(bool isError, const QString& hint);

	// 检查所有输入是否合法
	bool checkAllInput();

	void initUi();

private:
	// 邮箱正则
	static const QRegularExpression emailRegex;

	// 密码正则
	static const QRegularExpression passwordRegex;

private:
	Ui::RegisterDialog* ui;
	QMap<RequestType, std::function<void(const QJsonObject&)>> m_request_map;
};

#endif // REGISTERDIALOG_H
