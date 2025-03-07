#pragma once

#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>

class NotifyButton;
class ContactTreeWidget;

// 联系人列表(好友通知，群组通知，好友列表，群组列表)
class ContactWidget : public QWidget {
	Q_OBJECT
public:
	explicit ContactWidget(QWidget* parent = nullptr);

private:
	void initLayout();
	void initSignalSlots();

	// void addFriends();
	// void addGroups();

signals:
	void friendNotified();
	void groupNotified();

private:
	// 好友通知
	NotifyButton* m_notify_friend;
	// 群通知
	NotifyButton* m_notify_group;

	// 按钮组
	QButtonGroup* m_btn_grp;
	QPushButton* m_btn_friend;
	QPushButton* m_btn_group;

	// 好友列表
	ContactTreeWidget* m_friend_list;
	// 群聊列表
	ContactTreeWidget* m_group_list;
};