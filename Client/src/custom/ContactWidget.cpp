#include "custom/ContactWidget.h"
#include "custom/ContactTreeItem.h"
#include "custom/NotifyButton.h"
#include "custom/ContactTreeWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

ContactWidget::ContactWidget(QWidget* parent)
	: QWidget{parent},
	  m_notify_friend{new NotifyButton{this}},
	  m_notify_group{new NotifyButton{this}},
	  m_btn_grp{new QButtonGroup{this}},
	  m_btn_friend{new QPushButton{this}},
	  m_btn_group{new QPushButton{this}},
	  m_friend_list{new ContactTreeWidget{this}},
	  m_group_list{new ContactTreeWidget{this}} {
	initLayout();
	initSignalSlots();

	// TEST
	ContactTreeItem* item = new ContactTreeItem{m_friend_list};
	item->setAvatar(QPixmap{":/images/default_avatar.png"});
	item->setTitle("张三");
	item->setMessage("在吗？");
	item->setType(ContactTreeItem::Type::Friend);
	m_friend_list->addItemToGroup("144", item);

	ContactTreeItem* item2 = new ContactTreeItem{m_friend_list};
	item2->setAvatar(QPixmap{":/images/default_avatar.png"});
	item2->setTitle("李四");
	item2->setMessage("在吗？");
	item2->setType(ContactTreeItem::Type::Friend);
	m_friend_list->addItemToGroup("144", item2);
}

void ContactWidget::initLayout() {
	QVBoxLayout* main_layout = new QVBoxLayout{this};
	main_layout->setContentsMargins(5, 5, 5, 5);
	main_layout->setSpacing(0);

	// 好友通知
	m_notify_friend->setText("好友通知");
	m_notify_friend->setTrailingIcon(QIcon{":/images/collapse.png"});
	main_layout->addWidget(m_notify_friend);

	// 群通知
	m_notify_group->setText("群组通知");
	m_notify_group->setTrailingIcon(QIcon{":/images/collapse.png"});
	main_layout->addWidget(m_notify_group);

	// 按钮组
	m_btn_friend->setText("好友");
	m_btn_friend->setObjectName("btn_friend");

	m_btn_group->setText("群组");
	m_btn_group->setObjectName("btn_group");

	m_btn_grp->addButton(m_btn_friend, 0);
	m_btn_grp->addButton(m_btn_group, 1);
	for (auto& abtn : m_btn_grp->buttons()) {
		QPushButton* btn = static_cast<QPushButton*>(abtn);
		btn->setFixedHeight(40);
		btn->setFlat(true);
		btn->setCheckable(true);
		btn->setCursor(Qt::PointingHandCursor);
	}

	// 水平布局
	QHBoxLayout* btn_layout = new QHBoxLayout;
	btn_layout->setContentsMargins(2, 2, 2, 2);
	btn_layout->setSpacing(5);
	btn_layout->addWidget(m_btn_friend);
	btn_layout->addWidget(m_btn_group);
	main_layout->addLayout(btn_layout);

	// 好友列表
	main_layout->addWidget(m_friend_list);
	main_layout->addWidget(m_group_list);
	m_group_list->hide();
	m_friend_list->show();

	m_btn_friend->setChecked(true);
}

void ContactWidget::initSignalSlots() {
	connect(m_btn_grp, &QButtonGroup::idClicked, [this](int id) {
		if (id == 0) {
			m_friend_list->show();
			m_group_list->hide();
		} else {
			m_friend_list->hide();
			m_group_list->show();
		}
	});

	connect(m_notify_friend, &NotifyButton::clicked, this, &ContactWidget::friendNotified);
	connect(m_notify_group, &NotifyButton::clicked, this, &ContactWidget::groupNotified);
}
