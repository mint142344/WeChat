#include "RegisterDialog.h"
#include <qjsonobject.h>
#include "ui_RegisterDialog.h"
#include "HttpManager.h"

#include <QStyle>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
	ui->setupUi(this);

	// qss 属性设置
	ui->label_errorhint->setProperty("error", false);

	connect(HttpManager::getInstance(), &HttpManager::sig_module_register_finished, this,
			&RegisterDialog::on_request_finished);
}

RegisterDialog::~RegisterDialog() { delete ui; }

void RegisterDialog::setErrorHint(bool isError, const QString &hint) {
	ui->label_errorhint->setProperty("error", isError);
	ui->label_errorhint->setText(hint);
	// 运行时：重新加载样式
	ui->label_errorhint->style()->unpolish(ui->label_errorhint);
	ui->label_errorhint->style()->polish(ui->label_errorhint);
}

void RegisterDialog::on_lineEdit_email_editingFinished() {
	QString email = ui->lineEdit_email->text();
	// 正则检查邮箱
	static const QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
	bool isvalid = emailRegex.match(email).hasMatch();
	if (!isvalid)
		setErrorHint(true, "邮箱格式不正确");
	else
		setErrorHint(false, "");
}

void RegisterDialog::on_request_finished(RequestType request_type, const QJsonObject &json) {
	// 处理不同的请求
	m_request_map[request_type](json);
}

void RegisterDialog::registerCallback(const QJsonObject &json) {
	m_request_map.insert(RequestType::REGISTER, [this](const QJsonObject &json) {
		// 注册成功
	});

	m_request_map.insert(RequestType::GET_VERIFICATION_CODE, [this](const QJsonObject &json) {
		// 获取验证码成功
		bool ok = json["ok"].toBool();
		setErrorHint(false, "验证码已发送到邮箱");
	});
}
