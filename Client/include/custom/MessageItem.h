#pragma once

#include "Tool.h"
#include <QWidget>
#include <QEvent>
#include <QDateTime>
#include <QLabel>

// 侧边栏 消息列表 列表项
class MessageItem : public QWidget {
	Q_OBJECT
public:
	explicit MessageItem(QWidget* parent = nullptr);

	void setHasRead(bool read) { is_read = read; };
	bool hasRead() const { return is_read; };

	void setGroup(bool group) { is_group = group; }
	bool isGroup() const { return is_group; }

	void setTimeVisible(bool visible) { m_label_time->setVisible(visible); }
	bool isTimeVisible() const { return m_label_time->isVisible(); }

	void setNotDisturbVisible(bool visible) { m_label_icon->setVisible(visible); }
	bool isNotDisturbVisible() const { return m_label_icon->isVisible(); }

	void setPinTop(bool top) {
		is_pin_top = top;
		if (top) {
			m_label_title->setStyleSheet("font-size: 16px; color: #0066CC;");
		} else {
			m_label_title->setStyleSheet("font-size: 16px; color: #FFFFFF;");
		}
	}
	bool isPinTop() const { return is_pin_top; }

	// 设置 未读消息数 or 免打扰
	void setNotDisturb(bool disturb, int unread_cnt = 0);
	bool isNotDisturb() const { return is_disturb; }

	void setAvatar(const QUrl& url);
	void setTitle(const QString& title) { m_label_title->setText(title); }
	QString getTitle() const { return m_label_title->text(); }
	void setMessage(const QString& message) { m_label_message->setText(message); }
	void setTime(const QDateTime& time) { m_label_time->setText(Tool::formatTime(time)); }

	MessageItem* clone();

protected:
	// 鼠标进入事件
	void enterEvent(QEnterEvent* event) override;
	// 鼠标离开事件
	void leaveEvent(QEvent* event) override;

private:
	// 初始化控件布局
	void initLayout();

private:
	// 是否 已读
	bool is_read;
	// 是否 群聊
	bool is_group;
	// 是否 免打扰
	bool is_disturb;
	// 是否 置顶
	bool is_pin_top;

	// UI 控件
	QLabel* m_label_avatar;	 // 头像
	QLabel* m_label_title;	 // 标题
	QLabel* m_label_message; // 消息
	QLabel* m_label_time;	 // 时间

	//  未读消息数 or 免打扰
	QLabel* m_label_icon;
};
