#pragma once

#include <QPushButton>

// 通知 Item(Label + Trailing Icon)
class NotifyButton : public QPushButton {
	Q_OBJECT
public:
	explicit NotifyButton(QWidget* parent = nullptr);

	// 设置尾部图标
	void setTrailingIcon(const QIcon& icon) {
		setLayoutDirection(Qt::RightToLeft);
		setIcon(icon);
	};

private:
	void initLayout();
};
