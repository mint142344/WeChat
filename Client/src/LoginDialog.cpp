#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "RegisterDialog.h"
#include "DragWidgetFilter.h"
#include "custom/LineEdit.h"

#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QAction>

#include <memory>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
	ui->setupUi(this);
	initUi();

	if (QLineEdit* lineEdit = ui->comboBox_username->lineEdit()) {
		lineEdit->setPlaceholderText("请输入用户名");
	}

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
	// 用户名 leading action
	ui->comboBox_username->setLineEdit(new LineEdit(this));
	LineEdit* lineEdit = qobject_cast<LineEdit*>(ui->comboBox_username->lineEdit());

	if (lineEdit) {
		lineEdit->setLeadingAction(new QAction(QIcon(":/images/user.png"), "", lineEdit));
		lineEdit->setLeadingFocusAction(
			new QAction(QIcon(":/images/user-focus.png"), "", lineEdit));
	}

	// 密码 leading action
	ui->lineEdit_password->setLeadingAction(
		new QAction(QIcon(":/images/passwd.png"), "", ui->lineEdit_password));
	ui->lineEdit_password->setLeadingFocusAction(
		new QAction(QIcon(":/images/passwd-focus.png"), "", ui->lineEdit_password));

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
