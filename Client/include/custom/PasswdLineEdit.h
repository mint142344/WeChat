#pragma once

#include <QLineEdit>
#include <QPushButton>

class PasswdLineEdit : public QLineEdit {
	Q_OBJECT

public:
	explicit PasswdLineEdit(QWidget* parent = nullptr);
	// 设置附加功能 (先设置icon)
	void setAttachment(bool with_toggle_button, bool with_leading_action);

	// 设置 leading action icon
	void setLeadingIcon(const QString& icon, const QString& focus_icon);

	// 设置 visible button icon
	void setVisbleIcon(const QString& icon, const QString& hover_icon);
	void setInvisbleIcon(const QString& icon, const QString& hover_icon);

protected:
	// 设置 visible button
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;

	// 设置 leading action
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;

private:
	void updateToggleButtonIcon();

	// 初始化密码可见按钮
	void initVisibleButton();

	// 初始化 leading action
	void initLeadingAction();

private:
	QPushButton* m_toggle_button;
	QAction* m_leading_action = nullptr;

	bool m_visible = false;
	bool m_with_toggle_button = false;
	bool m_with_leading_action = false;

	QString m_leading_icon;
	QString m_leading_focus_icon;

	QString m_visible_icon;
	QString m_invisible_icon;
	QString m_visible_hover_icon;
	QString m_invisible_hover_icon;
};