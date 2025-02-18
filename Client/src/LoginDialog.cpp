#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "RegisterDialog.h"
#include "DragWidgetFilter.h"
#include "custom/PasswdLineEdit.h"

#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QAction>

#include <memory>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
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
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::initUi() {
	if (QLineEdit* lineEdit = ui->comboBox_username->lineEdit()) {
		lineEdit->setPlaceholderText("请输入用户名");
	}

	// 用户名 leading action
	ui->comboBox_username->setLineEdit(new PasswdLineEdit(this));
	PasswdLineEdit* passwdLineEdit =
		qobject_cast<PasswdLineEdit*>(ui->comboBox_username->lineEdit());
	if (passwdLineEdit) {
		passwdLineEdit->setLeadingIcon(":/images/user.png", ":/images/user-focus.png");
		passwdLineEdit->setAttachment(false, true);
	}

	// 密码 leading action
	ui->lineEdit_password->setLeadingIcon(":/images/passwd.png", ":/images/passwd-focus.png");
	ui->lineEdit_password->setVisbleIcon(":/images/passwd-visible.png",
										 ":/images/passwd-visible-hover.png");
	ui->lineEdit_password->setInvisbleIcon(":/images/passwd-invisible.png",
										   ":/images/passwd-invisible-hover.png");
	ui->lineEdit_password->setAttachment(true, true);

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
