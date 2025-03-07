#pragma once

#include <QWidget>

class QVBoxLayout;
class QButtonGroup;
class QToolButton;
class QLabel;

// 导航栏
class NavigationBar : public QWidget {
	Q_OBJECT

public:
	explicit NavigationBar(QWidget* parent = nullptr);

signals:
	// 头像 点击
	void sigAvatarClicked();
	// 消息按钮点击
	void sigMsgClicked();
	// 联系人按钮点击
	void sigContactClicked();
	// 机器人按钮点击
	void sigRobotClicked();
	// 设置按钮点击
	void sigSettingClicked();

private:
	// 初始化 控件布局
	void initLayout();

	// 初始化信号槽
	void initSignalSlots();

private:
	// 垂直布局
	QVBoxLayout* m_layout;
	// 程序名
	QLabel* m_title;
	// 头像 按钮
	QToolButton* m_btn_avatar;

	// 按钮组
	QButtonGroup* m_btn_group;
	// 聊天列表 按钮
	QToolButton* m_btn_msg;
	// 联系人 按钮
	QToolButton* m_btn_contact;
	// 机器人 按钮
	QToolButton* m_btn_robot;

	// 设置 按钮
	QToolButton* m_btn_setting;
};