#include "ForgetDialog.h"
#include "Common.h"
#include "ui_ForgetDialog.h"
#include "HttpManager.h"

#include <QStyle>
#include <QRegularExpression>
#include <QMessageBox>
#include <QJsonObject>

// 密码正则 6-15 位
const QRegularExpression ForgetDialog::passwordRegex("^[a-zA-Z0-9]{6,15}$");
// 邮箱正则
const QRegularExpression ForgetDialog::emailRegex(
	"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

ForgetDialog::ForgetDialog(QWidget* parent) : QDialog(parent), ui{new Ui::ForgetDialog} {
	ui->setupUi(this);
	initUi();

	registerAllCallback();
	connect(HttpManager::getInstance(), &HttpManager::sig_module_user_finished, this,
			&ForgetDialog::handle_http_response);
}

ForgetDialog::~ForgetDialog() { delete ui; }

void ForgetDialog::initUi() {
	ui->lineEdit_password->setVisbleIcon(":/images/passwd-visible.png",
										 ":/images/passwd-visible-hover.png");
	ui->lineEdit_password->setInvisbleIcon(":/images/passwd-invisible.png",
										   ":/images/passwd-invisible-hover.png");
	ui->lineEdit_password->setAttachment(true, false);

	ui->lineEdit_confirm_password->setVisbleIcon(":/images/passwd-visible.png",
												 ":/images/passwd-visible-hover.png");
	ui->lineEdit_confirm_password->setInvisbleIcon(":/images/passwd-invisible.png",
												   ":/images/passwd-invisible-hover.png");
	ui->lineEdit_confirm_password->setAttachment(true, false);

	ui->lineEdit_password->setToolTip("密码6-15位");
	ui->lineEdit_confirm_password->setToolTip("密码6-15位");

	// qss 属性设置
	ui->label_errorhint->setProperty("error", false);

	// 设置焦点 输入用户名
	ui->lineEdit_username->setFocus();
}

void ForgetDialog::registerAllCallback() {
	m_request_map[RequestType::FORGET_PASSWORD] = [this](const QJsonObject& json) {
		if (json["status"].toString() == "ok") {
			QMessageBox::information(this, "提示", "密码修改成功, 请重新登录");
			close();
		} else {
			setErrorHint(true, json["message"].toString());
		}
	};

	m_request_map[RequestType::GET_VERIFICATION_CODE] = [this](const QJsonObject& json) {
		if (json["status"].toString() == "ok") {
			setErrorHint(false, "验证码已发送到您的邮箱");
		} else {
			setErrorHint(true, json["message"].toString());
			ui->button_get_code->stopTimer();
			return;
		}
	};
}

void ForgetDialog::setErrorHint(bool isError, const QString& hint) {
	ui->label_errorhint->setProperty("error", isError);
	ui->label_errorhint->setText(hint);
	// 运行时：重新加载样式
	ui->label_errorhint->style()->unpolish(ui->label_errorhint);
	ui->label_errorhint->style()->polish(ui->label_errorhint);
}

bool ForgetDialog::checkAllInput() {
	// 1.username
	if (ui->lineEdit_username->text().isEmpty()) {
		setErrorHint(true, "用户名不能为空");
		return false;
	} else {
		setErrorHint(false, "");
	}

	// 2.email
	bool isvalid = emailRegex.match(ui->lineEdit_email->text()).hasMatch();
	if (!isvalid) {
		setErrorHint(true, "邮箱格式不正确");
		return false;
	} else {
		setErrorHint(false, "");
	}

	// 3.verify code
	if (ui->lineEdit_verify_code->text().isEmpty()) {
		setErrorHint(true, "验证码不能为空");
		return false;
	} else {
		setErrorHint(false, "");
	}

	// 4.new passwd
	isvalid = passwordRegex.match(ui->lineEdit_password->text()).hasMatch();
	if (!isvalid) {
		setErrorHint(true, "密码至少6位");
		return false;
	} else {
		setErrorHint(false, "");
	}

	// 5.confirm passwd
	if (ui->lineEdit_password->text() != ui->lineEdit_confirm_password->text()) {
		setErrorHint(true, "两次密码不一致");
		return false;
	} else {
		setErrorHint(false, "");
	}

	return true;
}

void ForgetDialog::handle_http_response(RequestType request_type, const QJsonObject& json) {
	// 处理不同的请求
	try {
		m_request_map[request_type](json);
	} catch (std::exception& e) {
		qDebug() << __PRETTY_FUNCTION__ << __LINE__ << e.what();
	}
}

void ForgetDialog::on_button_confirm_clicked() {
	qDebug() << "ForgetDialog::on_button_confirm_clicked";

	if (!checkAllInput()) {
		return;
	}

	QJsonObject json;
	json["username"] = ui->lineEdit_username->text();
	json["email"] = ui->lineEdit_email->text();
	json["verify_code"] = ui->lineEdit_verify_code->text();
	json["password"] = ui->lineEdit_password->text();

	// 发送请求
	HttpManager::getInstance()->post("/forget_password", json, ModuleType::USER,
									 RequestType::FORGET_PASSWORD);
}

void ForgetDialog::on_button_get_code_clicked() {
	qDebug() << "ForgetDialog::on_button_get_code_clicked";

	QString email = ui->lineEdit_email->text();
	if (emailRegex.match(email).hasMatch()) {
		setErrorHint(false, "");
	} else {
		setErrorHint(true, "邮箱格式不正确");
		return;
	}

	QJsonObject json;
	json["email"] = email;

	// 发送请求
	HttpManager::getInstance()->post("/verify_code", json, ModuleType::USER,
									 RequestType::GET_VERIFICATION_CODE);

	ui->button_get_code->startTimer();
}
