#pragma once

#include <QWidget>
#include <QLabel>

// 联系人列表项：好友、群聊(无个人简介)
class ContactTreeItem : public QWidget {
	Q_OBJECT

public:
	enum Type {
		Friend,
		Group,
	};

public:
	explicit ContactTreeItem(QWidget* parent = nullptr);

	void setAvatar(const QPixmap& avatar) { m_label_avatar->setPixmap(avatar); }
	void setTitle(const QString& title) { m_label_title->setText(title); }
	void setMessage(const QString& message) { m_label_message->setText(message); }

	void setType(Type type) { m_tpye = type; }
	Type getType() const { return m_tpye; }

	void setGroupName(const QString& name) { m_group_name = name; }
	QString getGroupName() const { return m_group_name; }

private:
	void initLayout();

private:
	QLabel* m_label_avatar;
	QLabel* m_label_title;
	QLabel* m_label_message;

	QString m_group_name;

	Type m_tpye = Friend;
};