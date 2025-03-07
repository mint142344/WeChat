#include "ChatWidget/NavigationBar.h"

#include <QVBoxLayout>
#include <QButtonGroup>
#include <QToolButton>
#include <QLabel>

NavigationBar::NavigationBar(QWidget* parent)
	: QWidget{parent},
	  m_layout(new QVBoxLayout{this}),
	  m_title(new QLabel{"WeQQ", this}),
	  m_btn_group(new QButtonGroup{this}),
	  m_btn_avatar(new QToolButton{this}),
	  m_btn_msg(new QToolButton{this}),
	  m_btn_contact(new QToolButton{this}),
	  m_btn_robot(new QToolButton{this}),
	  m_btn_setting(new QToolButton{this}) {
	initLayout();
	initSignalSlots();
	m_btn_msg->setChecked(true);
}

void NavigationBar::initLayout() {
	// 固定宽度
	setFixedWidth(60);

	// 主垂直布局
	m_layout->setContentsMargins(0, 5, 0, 15); // 左上右下
	m_layout->setAlignment(Qt::AlignCenter);
	m_layout->setSpacing(15);

	// 上部区域
	m_btn_avatar->setIcon(QIcon(":/images/default_avatar.png")); // 头像 按钮
	m_btn_avatar->setFixedSize(60, 60);
	m_btn_avatar->setIconSize(QSize(50, 50));
	m_btn_avatar->setObjectName("btn_avatar");
	m_layout->addWidget(m_title);
	m_layout->addWidget(m_btn_avatar);

	// 中间区域
	QVBoxLayout* mid_vlayout = new QVBoxLayout; // 中间垂直布局
	mid_vlayout->setContentsMargins(0, 0, 0, 0);
	mid_vlayout->setSpacing(15);
	mid_vlayout->setAlignment(Qt::AlignCenter);

	m_btn_msg->setObjectName("btn_msg"); // 聊天列表 按钮
	m_btn_msg->setToolTip("消息");
	m_btn_msg->setIcon(QIcon(":/images/btn_chat.png"));

	m_btn_contact->setObjectName("btn_contact"); // 联系人 按钮
	m_btn_contact->setToolTip("联系人");
	m_btn_contact->setIcon(QIcon(":/images/btn_contact.png"));

	m_btn_robot->setObjectName("btn_robot"); // 机器人 按钮
	m_btn_robot->setToolTip("机器人");
	m_btn_robot->setIcon(QIcon(":/images/btn_robot.png"));

	mid_vlayout->addWidget(m_btn_msg);
	mid_vlayout->addWidget(m_btn_contact);
	mid_vlayout->addWidget(m_btn_robot);
	m_layout->addLayout(mid_vlayout);

	// 添加按钮到按钮组
	m_btn_group->setExclusive(true);
	m_btn_group->addButton(m_btn_msg);
	m_btn_group->addButton(m_btn_contact);
	m_btn_group->addButton(m_btn_robot);
	for (auto* btn : m_btn_group->buttons()) {
		btn->setFixedSize(40, 40);
		btn->setIconSize(QSize(28, 28));
		btn->setCheckable(true);
		btn->setCursor(Qt::PointingHandCursor);
	}

	// 底部区域
	m_layout->addStretch(1);
	m_btn_setting->setObjectName("btn_setting");
	m_btn_setting->setToolTip("设置");
	m_btn_setting->setCursor(Qt::PointingHandCursor);
	m_layout->addWidget(m_btn_setting);
}

void NavigationBar::initSignalSlots() {
	// toggle
	connect(m_btn_msg, &QToolButton::toggled, [this](bool checked) {
		m_btn_msg->setIcon(checked ? QIcon(":/images/btn_chat_checked.png")
								   : QIcon(":/images/btn_chat.png"));
	});
	connect(m_btn_contact, &QToolButton::toggled, [this](bool checked) {
		m_btn_contact->setIcon(checked ? QIcon(":/images/btn_contact_checked.png")
									   : QIcon(":/images/btn_contact.png"));
	});
	connect(m_btn_robot, &QToolButton::toggled, [this](bool checked) {
		m_btn_robot->setIcon(checked ? QIcon(":/images/btn_robot_checked.png")
									 : QIcon(":/images/btn_robot.png"));
	});

	// 点击
	connect(m_btn_avatar, &QToolButton::clicked, this, &NavigationBar::sigAvatarClicked);
	connect(m_btn_msg, &QToolButton::clicked, this, &NavigationBar::sigMsgClicked);
	connect(m_btn_contact, &QToolButton::clicked, this, &NavigationBar::sigContactClicked);
	connect(m_btn_robot, &QToolButton::clicked, this, &NavigationBar::sigRobotClicked);
	connect(m_btn_setting, &QToolButton::clicked, this, &NavigationBar::sigSettingClicked);
}
