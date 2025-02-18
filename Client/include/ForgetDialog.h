#pragma once

#include "Common.h"

#include <QDialog>

// 前置声明(在cpp中include ui_xxx)
namespace Ui {
class ForgetDialog;
}

class ForgetDialog : public QDialog {
	Q_OBJECT

public:
	explicit ForgetDialog(QWidget* parent = nullptr);
	~ForgetDialog();

private:
	void initUi();
	// 注册网络请求回调
	void registerAllCallback();
	// 设置 UI 错误提示
	void setErrorHint(bool isError, const QString& hint);
	// 检查所有输入是否合法
	bool checkAllInput();

private slots:
	// 获取验证码
	void on_button_get_code_clicked();
	// 找回密码
	void on_button_confirm_clicked();
	// 收到网络请求 执行回调
	void handle_http_response(RequestType request_type, const QJsonObject& json);

private:
	Ui::ForgetDialog* ui;

	QMap<RequestType, std::function<void(const QJsonObject&)>> m_request_map;

	// 邮箱正则
	static const QRegularExpression emailRegex;
	// 密码正则
	static const QRegularExpression passwordRegex;
};