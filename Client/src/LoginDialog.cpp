#include "Common.h"
#include "LoginDialog.h"
#include "LoadingDialog.h"
#include "ui_LoginDialog.h"
#include "ForgetDialog.h"
#include "RegisterDialog.h"
#include "DragWidgetFilter.h"
#include "custom/PasswdLineEdit.h"
#include "HttpManager.h"
#include "TcpClient.h"

#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QAction>
#include <QJsonObject>
#include <QMessageBox>

#include <memory>

LoginDialog::LoginDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::LoginDialog), m_loading_dialog(new LoadingDialog(this)) {
	ui->setupUi(this);
	initUi();

	// 无边框
	setWindowFlag(Qt::FramelessWindowHint, true);
	setAttribute(Qt::WA_TranslucentBackground);

	// 设置阴影
	// setShadow();

	// 安装拖拽事件过滤器
	installEventFilter(new DragWidgetFilter(this));

	// 注册跳转
	connect(ui->label_register, &QLabel::linkActivated, [this](const QString&) {
		std::unique_ptr<RegisterDialog> registerDialog = std::make_unique<RegisterDialog>(this);
		registerDialog->exec();
	});

	// 忘记密码跳转
	connect(ui->label_forgot_password, &QLabel::linkActivated, [this](const QString&) {
		std::unique_ptr<ForgetDialog> forgetDialog = std::make_unique<ForgetDialog>(this);
		forgetDialog->exec();
	});

	// 关联登录到ChatServer
	connect(TcpClient::getInstance(), &TcpClient::loggedChatServer,
			[this](bool success, const QString& message) {
				m_loading_dialog->close();

				if (success) {
					accept();
				} else {
					setErrorHint(true, message);
					QMessageBox::warning(this, "登录失败", message);
				}
			});

	// 登录 GateServer 请求回调
	connect(HttpManager::getInstance(), &HttpManager::sig_module_login_finished,
			[this](RequestType type, const QJsonObject& json) {
				if (json["status"].toString() == "ok") {
					auto* tcp = TcpClient::getInstance();
					tcp->setProperty("id", json["id"].toInt());
					tcp->setProperty("token", json["token"].toString());

					// 登录 ChatServer
					tcp->loginToHost(json["host"].toString(), json["port"].toInt());
					// 阻塞等待登录结果
					m_loading_dialog->exec();
				} else {
					// 登录失败
					setErrorHint(true, json["message"].toString());
				}
			});
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::on_button_login_clicked() {
	if (ui->comboBox_username->currentText().isEmpty()) {
		setErrorHint(true, "用户名不能为空");
		return;
	}
	setErrorHint(false, "");

	if (ui->lineEdit_password->text().isEmpty()) {
		setErrorHint(true, "密码不能为空");
		return;
	}
	setErrorHint(false, "");

	QJsonObject json;
	json["username"] = ui->comboBox_username->currentText();
	json["password"] = ui->lineEdit_password->text();

	HttpManager::getInstance()->post("/login", json, ModuleType::LOGIN, RequestType::LOGIN);

	// qDebug() << "LoginDialog::on_button_login_clicked";
}

void LoginDialog::initUi() {
	// 用户名 leading action
	ui->comboBox_username->setLineEdit(new PasswdLineEdit(this));
	PasswdLineEdit* passwdLineEdit =
		qobject_cast<PasswdLineEdit*>(ui->comboBox_username->lineEdit());
	if (passwdLineEdit != nullptr) {
		passwdLineEdit->setLeadingIcon(":/images/user.png", ":/images/user-focus.png");
		passwdLineEdit->setAttachment(false, true);
		passwdLineEdit->setPlaceholderText("请输入用户名");
		passwdLineEdit->setFont(QFont("Microsoft YaHei", 12));
	}

	// 密码 leading action
	ui->lineEdit_password->setLeadingIcon(":/images/passwd.png", ":/images/passwd-focus.png");
	ui->lineEdit_password->setVisbleIcon(":/images/passwd-visible.png",
										 ":/images/passwd-visible-hover.png");
	ui->lineEdit_password->setInvisbleIcon(":/images/passwd-invisible.png",
										   ":/images/passwd-invisible-hover.png");
	ui->lineEdit_password->setAttachment(true, true);
	ui->lineEdit_password->setFont(QFont("Microsoft YaHei", 12));

	ui->comboBox_username->setFocus();
}

void LoginDialog::setShadow() {
	// 创建主阴影效果
	QGraphicsDropShadowEffect* mainShadow = new QGraphicsDropShadowEffect(this);

	mainShadow->setBlurRadius(20);
	mainShadow->setColor(QColor(180, 180, 180));
	mainShadow->setOffset(0, 0);

	// 设置主阴影
	ui->frame_container->setGraphicsEffect(mainShadow);
}

void LoginDialog::setErrorHint(bool isError, const QString& hint) {
	ui->label_errorhint->setProperty("error", isError);
	ui->label_errorhint->setText(hint);
	// 运行时：重新加载样式
	ui->label_errorhint->style()->unpolish(ui->label_errorhint);
	ui->label_errorhint->style()->polish(ui->label_errorhint);
}
