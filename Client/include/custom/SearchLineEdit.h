#pragma once

#include <qaction.h>
#include <QLineEdit>
#include <QAction>
#include <QFocusEvent>

class SearchLineEdit : public QLineEdit {
	Q_OBJECT

public:
	explicit SearchLineEdit(QWidget* parent = nullptr);

	// 设置搜索图标
	void setSearchIcon(const QIcon& icon);

	// 设置清除按钮图标
	void setClearIcon(const QIcon& icon);

protected:
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;

private:
	QAction* m_search_action{}; // 搜索
	QAction* m_clear_action{};	// 清除
};
