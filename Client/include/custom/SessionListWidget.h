#pragma once

#include <QListWidget>
#include "custom/SessionItem.h"

// robot 会话列表控件
class SessionListWidget : public QListWidget {
	Q_OBJECT
public:
	explicit SessionListWidget(QWidget* parent = nullptr);

	void addSessionItem(SessionItem* item);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
};