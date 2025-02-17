#include "LoginDialog.h"
#include "ui_LoginDialog.h"
#include "RegisterDialog.h"
#include "DragWidgetFilter.h"

#include <memory>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginDialog) {
	ui->setupUi(this);
	if (QLineEdit* lineEdit = ui->comboBox_username->lineEdit()) {
		lineEdit->setPlaceholderText("请输入用户名");
	}

	// 无边框
	setWindowFlag(Qt::FramelessWindowHint, true);

	// 安装拖拽事件过滤器
	installEventFilter(new DragWidgetFilter(this));

	// 注册跳转
	connect(ui->label_register, &QLabel::linkActivated, [this](const QString&) {
		std::unique_ptr<RegisterDialog> registerDialog = std::make_unique<RegisterDialog>(this);
		registerDialog->exec();
	});
}

LoginDialog::~LoginDialog() { delete ui; }
