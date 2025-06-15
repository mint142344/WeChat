#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "AddFriendDialog.h"
#include "custom/SearchLineEdit.h"

#include <QFile>
#include <QMessageBox>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent)
	: QWidget(parent),
	  ui(new Ui::MainWindow),
	  m_btn_group(new QButtonGroup{this}),
	  m_add_menu(new QMenu{this}) {
	ui->setupUi(this);
	initUi();
	initSignalSlots();

	switchTheme(":/style/themes/dark.qss");

	// 启动显示聊天界面
	ui->btn_chat->setChecked(true);
	buttonClicked(0);
	ui->search_list->hide();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::switchTheme(const QString& path) {
	QFile file(path);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		qDebug() << "Can't open the file!";
		return;
	}

	QString styleSheet = file.readAll();
	setStyleSheet(styleSheet);
}

void MainWindow::initUi() {
	// 隐藏群聊信息栏
	ui->group_bar->hide();

	// 导航栏按钮
	ui->btn_avatar->setIconSize(ui->btn_avatar->size());
	ui->btn_avatar->setIcon(QIcon(":/images/default_avatar.png"));
	ui->btn_chat->setIcon(QIcon(":/images/btn_chat_checked.png"));
	ui->btn_contact->setIcon(QIcon(":/images/btn_contact.png"));
	ui->btn_robot->setIcon(QIcon(":/images/btn_robot.png"));

	m_btn_group->addButton(ui->btn_chat, 0);
	m_btn_group->addButton(ui->btn_contact, 1);
	m_btn_group->addButton(ui->btn_robot, 2);

	for (auto* btn : m_btn_group->buttons()) {
		btn->setIconSize(QSize(24, 24));
	}

	// Splitter
	ui->splitter_main->setStretchFactor(0, 1);
	ui->splitter_main->setStretchFactor(1, 3);
	ui->splitter_main->setCollapsible(0, false);
	ui->splitter_chat->setStretchFactor(0, 3);
	ui->splitter_chat->setStretchFactor(1, 1);

	// 侧边栏 搜索栏
	ui->btn_add->setIcon(QIcon(":/images/btn_add.png"));
	ui->btn_add->setIconSize(ui->btn_add->size());
	ui->search->addAction(QIcon(":/images/btn_search.png"), QLineEdit::LeadingPosition);
	ui->btn_add->setIconSize(QSize(16, 16));
	ui->btn_add->setIcon(QIcon(":/images/btn_add_friend.png"));

	// 添加好友/群 菜单
	m_add_menu->addAction("添加好友/群", this, &MainWindow::addFriendOrGroup);
	m_add_menu->addAction("创建群聊", this, &MainWindow::createGroup);
	ui->btn_add->setMenu(m_add_menu);
}

void MainWindow::initSignalSlots() {
	// 导航栏按钮组
	connect(m_btn_group, &QButtonGroup::idClicked, this, &MainWindow::buttonClicked);

	// 侧边栏搜索栏
	connect(ui->search, &SearchLineEdit::focusIn, [this] {
		m_id_checked.push({{0, ui->msg_list->isVisible()},
						   {1, ui->contact_list->isVisible()},
						   {2, ui->session_list->isVisible()}});
		ui->msg_list->hide();
		ui->contact_list->hide();
		ui->session_list->hide();
		ui->search_list->show();
	});
	connect(ui->search, &SearchLineEdit::focusOut, [this] {
		ui->search_list->hide();
		if (!m_id_checked.isEmpty()) {
			auto checked = m_id_checked.pop();
			for (auto it = checked.begin(); it != checked.end(); ++it) {
				if (it.value()) {
					buttonClicked(it.key());
					break;
				}
			}
		}
	});

	connect(ui->btn_toggle_group, &QToolButton::toggled, [this](bool checked) {
		int width = ui->group_bar->width();
		if (checked) {
			ui->group_bar->show();
			resize(size() + QSize(width, 0));
		} else {
			ui->group_bar->hide();
			resize(size() - QSize(width, 0));
		}
	});
}

void MainWindow::on_btn_avatar_clicked() {
	QMessageBox::information(this, "Avatar", "Avatar clicked");
}

void MainWindow::on_btn_setting_clicked() {
	QMessageBox::information(this, "Setting", "Setting clicked");
}

void MainWindow::on_btn_chat_toggled(bool checked) {
	ui->btn_chat->setIcon(checked ? QIcon(":/images/btn_chat_checked.png")
								  : QIcon(":/images/btn_chat.png"));
}

void MainWindow::on_btn_contact_toggled(bool checked) {
	ui->btn_contact->setIcon(checked ? QIcon(":/images/btn_contact_checked.png")
									 : QIcon(":/images/btn_contact.png"));
}

void MainWindow::on_btn_robot_toggled(bool checked) {
	ui->btn_robot->setIcon(checked ? QIcon(":/images/btn_robot_checked.png")
								   : QIcon(":/images/btn_robot.png"));
}

void MainWindow::addFriendOrGroup() {
	AddFriendDialog dialog;
	dialog.exec();
}

void MainWindow::createGroup() { qInfo() << "TODO:Create group"; }

void MainWindow::buttonClicked(int id) {
	ui->search->clearFocus();
	switch (id) {
		// 聊天
		case 0:
			ui->msg_list->show();
			ui->contact_list->hide();
			ui->session_list->hide();
			break;

			// 联系人
		case 1:
			ui->msg_list->hide();
			ui->contact_list->show();
			ui->session_list->hide();
			break;

			// 机器人
		case 2:
			ui->msg_list->hide();
			ui->contact_list->hide();
			ui->session_list->show();
			break;
	}
}
