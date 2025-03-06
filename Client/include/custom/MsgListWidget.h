#pragma once

#include <QListWidget>
#include "custom/MessageItem.h"

// 消息列表控件
class MsgListWidget : public QListWidget {
	Q_OBJECT
public:
	explicit MsgListWidget(QWidget* parent = nullptr);

	void addMessageItem(MessageItem* item);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;

private:
};