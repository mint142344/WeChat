#include "RegisterDialog.h"
#include "Common.h"
#include "ui_RegisterDialog.h"
#include "HttpManager.h"
#include "custom/PasswdLineEdit.h"

#include <QStyle>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QJsonObject>
#include <QMessageBox>

// 密码正则 6-15 位
const QRegularExpression RegisterDialog::passwordRegex("^[a-zA-Z0-9]{6,15}$");
// 邮箱正则
const QRegularExpression RegisterDialog::emailRegex(
	"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent), ui(new Ui::RegisterDialog) {
	ui->setupUi(this);
	initUi();

	// 注册网络请求回调
	registerAllCallback();

	connect(HttpManager::getInstance(), &HttpManager::sig_module_register_finished, this,
			&RegisterDialog::http_request_finished);
}

RegisterDialog::~RegisterDialog() { delete ui; }

void RegisterDialog::setErrorHint(bool isError, const QString& hint) {
	ui->label_errorhint->setProperty("error", isError);
	ui->label_errorhint->setText(hint);
	// 运行时：重新加载样式
	ui->label_errorhint->style()->unpolish(ui->label_errorhint);
	ui->label_errorhint->style()->polish(ui->label_errorhint);
}

bool RegisterDialog::checkAllInput() {
	if (ui->lineEdit_username->text().isEmpty()) {
		setErrorHint(true, "用户名不能为空");
		return false;
	} else {
		setErrorHint(false, "");
	}

	bool isvalid = emailRegex.match(ui->lineEdit_email->text()).hasMatch();
	if (!isvalid) {
		setErrorHint(true, "邮箱格式不正确");
		return false;
	} else {
		setErrorHint(false, "");
	}

	if (ui->lineEdit_password->text().isEmpty()) {
		setErrorHint(true, "密码不能为空");
		return false;
	} else {
		setErrorHint(false, "");
	}

	if (ui->lineEdit_password->text() != ui->lineEdit_confirmPassword->text()) {
		setErrorHint(true, "两次密码不一致");
		return false;
	} else {
		setErrorHint(false, "");
	}

	isvalid = passwordRegex.match(ui->lineEdit_password->text()).hasMatch();
	if (!isvalid) {
		setErrorHint(true, "密码至少6位");
		return false;
	} else {
		setErrorHint(false, "");
	}

	if (ui->lineEdit_verificationCode->text().isEmpty()) {
		setErrorHint(true, "验证码不能为空");
		return false;
	} else {
		setErrorHint(false, "");
	}

	return true;
}

void RegisterDialog::initUi() {
	ui->lineEdit_password->setToolTip("密码6-15位");
	ui->lineEdit_confirmPassword->setToolTip("密码6-15位");

	ui->lineEdit_password->setVisbleIcon(":/images/passwd-visible.png",
										 ":/images/passwd-visible-hover.png");
	ui->lineEdit_password->setInvisbleIcon(":/images/passwd-invisible.png",
										   ":/images/passwd-invisible-hover.png");
	ui->lineEdit_password->setAttachment(true, false);

	ui->lineEdit_confirmPassword->setVisbleIcon(":/images/passwd-visible.png",
												":/images/passwd-visible-hover.png");
	ui->lineEdit_confirmPassword->setInvisbleIcon(":/images/passwd-invisible.png",
												  ":/images/passwd-invisible-hover.png");
	ui->lineEdit_confirmPassword->setAttachment(true, false);

	// qss 属性设置
	ui->label_errorhint->setProperty("error", false);
}

void RegisterDialog::on_lineEdit_email_editingFinished() {
	QString email = ui->lineEdit_email->text();
	// 正则检查邮箱
	bool isvalid = emailRegex.match(email).hasMatch();
	if (!isvalid)
		setErrorHint(true, "邮箱格式不正确");
	else
		setErrorHint(false, "");
}

void RegisterDialog::on_lineEdit_password_editingFinished() {
	// 不少6位的密码
	bool isvalid = passwordRegex.match(ui->lineEdit_password->text()).hasMatch();
	if (!isvalid)
		setErrorHint(true, "密码至少6位");
	else
		setErrorHint(false, "");
}

void RegisterDialog::on_button_confirm_clicked() {
	if (!checkAllInput()) return;

	QJsonObject json;
	json["username"] = ui->lineEdit_username->text();
	json["password"] = ui->lineEdit_password->text();
	json["email"] = ui->lineEdit_email->text();
	json["verify_code"] = ui->lineEdit_verificationCode->text();

	HttpManager::getInstance()->post("/register", json, ModuleType::REGISTER,
									 RequestType::NEW_USER);
}

void RegisterDialog::http_request_finished(RequestType request_type, const QJsonObject& json) {
	// 处理不同的请求
	try {
		m_request_map[request_type](json);
	} catch (std::exception& e) {
		qDebug() << __PRETTY_FUNCTION__ << __LINE__ << e.what();
	}
}

void RegisterDialog::registerAllCallback() {
	m_request_map.insert(RequestType::NEW_USER, [this](const QJsonObject& json) {
		// 注册结果
		qDebug() << "registerAllCallback" << __LINE__ << json;
		if (json["status"].toString() == "error") {
			setErrorHint(true, "注册失败");
			QMessageBox::warning(this, "注册失败", json["message"].toString());
			return;
		} else {
			setErrorHint(false, "注册成功");
			QMessageBox::information(this, "注册成功", "点击返回登录");
			accept();
		}
	});

	m_request_map.insert(RequestType::GET_VERIFICATION_CODE, [this](const QJsonObject& json) {
		// 验证码发送结果
		qDebug() << "registerAllCallback" << __LINE__ << json;
		if (json["status"].toString() == "error") {
			setErrorHint(true, json["message"].toString());
			ui->button_get_code->stopTimer();
			return;
		} else {
			setErrorHint(false, "验证码已发送到邮箱");
		}
	});
}

void RegisterDialog::on_button_get_code_clicked() {
	QString email = ui->lineEdit_email->text();

	// 正则检查邮箱
	static const QRegularExpression emailRegex("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
	bool isvalid = emailRegex.match(email).hasMatch();

	if (!isvalid) {
		setErrorHint(true, "邮箱格式不正确");
		return;
	} else {
		setErrorHint(false, "");
	}

	QJsonObject json;
	json["email"] = email;

	HttpManager::getInstance()->post("/verify_code", json, ModuleType::REGISTER,
									 RequestType::GET_VERIFICATION_CODE);
	// 开始计时
	ui->button_get_code->startTimer();
}
