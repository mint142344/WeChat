#pragma once
#include "Tool.h"
#include <QWidget>
#include <QLabel>
#include <QStyle>
#include <QApplication>

// robot 会话列表 列表项(比消息列表项窄)
class SessionItem : public QWidget {
	Q_OBJECT
public:
	explicit SessionItem(QWidget* parent = nullptr);

	// 编辑名称
	void setTitle(const QString& title) { m_label_title->setText(title); }
	QString getTitle() const { return m_label_title->text(); }

	void setTime(const QDateTime& time) { m_label_time->setText(Tool::formatTime(time)); }
	void setPinTop(bool top) {
		m_pin_top = top;
		if (m_pin_top) {
			m_label_title->setStyleSheet("font-size: 16px; color: #0066CC;");
		} else {
			m_label_title->setStyleSheet("font-size: 16px; color: #FFFFFF;");
		}
	}
	bool isPinnedTop() const { return m_pin_top; }

	// 设置唯一标识
	void setUUID(const QString& uuid) { m_uuid = uuid; }
	QString getUUID() const { return m_uuid; }

	// 克隆
	SessionItem* clone();

private:
	// 初始化控件布局
	void initLayout();

private:
	QLabel* m_label_title;
	QLabel* m_label_time;
	bool m_pin_top{};

	QString m_uuid;
};