#include "ChatWidget/SiderBarWidget.h"

#include <QAction>
#include <QHBoxLayout>

SiderBarWidget::SiderBarWidget(QWidget* parent)
	: QWidget(parent),
	  m_layout{new QVBoxLayout{this}},
	  m_search_edit{new SearchLineEdit{this}},
	  m_btn_add{new QToolButton{this}},
	  m_view_cur{ViewType::MessageView},
	  m_view_msg{new MsgListWidget{this}},
	  m_view_contact{new QTreeWidget{this}},
	  m_view_robot{new SessionListWidget{this}} {
	initLayout();
	initSignalSlots();

	for (int i = 0; i < 50; ++i) {
		// msg
		MessageItem* item1 = new MessageItem;
		item1->setAvatar(QPixmap(":/images/default_avatar.png"));
		item1->setTitle(QString("mint flower %1").arg(i));
		item1->setMessage("hello world");
		item1->setTime(QDateTime::currentDateTime());
		item1->setNotDisturb(true);

		m_view_msg->addMessageItem(item1);

		// session
		SessionItem* item2 = new SessionItem;
		item2->setTitle(QString("%1-在上述三种方法中，第一种（纯样式表方法").arg(i));
		item2->setTime(QDateTime::currentDateTime());

		m_view_robot->addSessionItem(item2);
	}
}

void SiderBarWidget::switchToView(ViewType type) {
	m_view_cur = type;
	m_view_msg->hide();
	m_view_contact->hide();
	m_view_robot->hide();

	switch (type) {
		case ViewType::MessageView:
			m_view_msg->show();
			break;
		case ViewType::ContactView:
			m_view_contact->show();
			break;
		case ViewType::RobotView:
			m_view_robot->show();
			break;
	}
}

void SiderBarWidget::setMinMaxWidth(int min, int max) {
	// 搜索框
	m_search_edit->setMinimumWidth(min - 50);
	m_search_edit->setMaximumWidth(max - 50);

	// 消息列表
	m_view_msg->setMinimumWidth(min);
	m_view_msg->setMaximumWidth(max);

	// 联系人列表
	m_view_contact->setMinimumWidth(min);
	m_view_contact->setMaximumWidth(max);

	// 机器人会话列表
	m_view_robot->setMinimumWidth(min);
	m_view_robot->setMaximumWidth(max);

	setMinimumWidth(min);
	setMaximumWidth(max);
}

void SiderBarWidget::initLayout() {
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(5);

	QHBoxLayout* search_layout = new QHBoxLayout;
	search_layout->setContentsMargins(10, 20, 10, 10);
	search_layout->setSpacing(10);

	// 上部分
	m_search_edit->setSearchIcon(QIcon(":/images/btn_search.png"));
	m_search_edit->setClearIcon(QIcon(":/images/btn_clear.png"));
	m_search_edit->setPlaceholderText(tr("搜索"));
	m_search_edit->setFixedHeight(30);

	m_btn_add->setObjectName("btn_add");
	m_btn_add->setFixedSize(30, 30);
	m_btn_add->setIcon(QIcon(":/images/btn_add_friend.png"));
	m_btn_add->setIconSize(QSize(20, 20));
	m_btn_add->setToolTip("添加好友");

	search_layout->addWidget(m_search_edit);
	search_layout->addWidget(m_btn_add);
	m_layout->addLayout(search_layout);

	// 下部分
	m_layout->addWidget(m_view_msg);
	m_layout->addWidget(m_view_contact);
	m_layout->addWidget(m_view_robot);

	setLayout(m_layout);

	// 设置默认视图
	switchToView(ViewType::MessageView);
}

void SiderBarWidget::initSignalSlots() {
	// 查找 与 添加
	connect(m_btn_add, &QToolButton::clicked, this, &SiderBarWidget::addButtonClicked);
	connect(m_search_edit, &SearchLineEdit::textChanged, this, &SiderBarWidget::searchTextChanged);

	// TODO:item clicked
}
