#pragma once

#include "Tool.h"
#include <QWidget>
#include <QLabel>

// robot 会话列表 列表项(比消息列表项窄)
class SessionItem : public QWidget {
	Q_OBJECT
public:
	explicit SessionItem(QWidget* parent = nullptr);

	// 编辑名称
	void setTitle(const QString& title) { m_label_title->setText(title); }
	void setTime(const QDateTime& time) { m_label_time->setText(Tool::formatTime(time)); }

protected:
	// 鼠标进入事件
	void enterEvent(QEnterEvent* event) override;
	// 鼠标离开事件
	void leaveEvent(QEvent* event) override;

private:
	// 初始化控件布局
	void initLayout();

private:
	QLabel* m_label_title;
	QLabel* m_label_time;
};