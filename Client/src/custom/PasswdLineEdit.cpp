#include "custom/PasswdLineEdit.h"

#include <QHBoxLayout>
#include <QStyle>
#include <QFocusEvent>

PasswdLineEdit::PasswdLineEdit(QWidget* parent) : QLineEdit(parent) {}

void PasswdLineEdit::setAttachment(bool with_toggle_button, bool with_leading_action) {
	m_with_leading_action = with_leading_action;
	m_with_toggle_button = with_toggle_button;

	if (m_with_toggle_button) {
		assert(!m_visible_icon.isEmpty() || !m_invisible_icon.isEmpty() ||
			   !m_visible_hover_icon.isEmpty() || !m_invisible_hover_icon.isEmpty());

		initVisibleButton();
	}

	if (m_with_leading_action) {
		assert(!m_leading_icon.isEmpty() || !m_leading_focus_icon.isEmpty());

		initLeadingAction();
	}
}

void PasswdLineEdit::setLeadingIcon(const QString& icon, const QString& focus_icon) {
	m_leading_icon = icon;
	m_leading_focus_icon = focus_icon;
}

void PasswdLineEdit::setVisbleIcon(const QString& icon, const QString& hover_icon) {
	m_visible_icon = icon;
	m_visible_hover_icon = hover_icon;
}

void PasswdLineEdit::setInvisbleIcon(const QString& icon, const QString& hover_icon) {
	m_invisible_hover_icon = hover_icon;
	m_invisible_icon = icon;
}

void PasswdLineEdit::updateToggleButtonIcon() {
	if (m_visible) {
		m_toggle_button->setIcon(QIcon(m_visible_icon));
	} else {
		m_toggle_button->setIcon(QIcon(m_invisible_icon));
	}
}

void PasswdLineEdit::initVisibleButton() {
	// 设置 密码可见 按钮
	m_toggle_button = new QPushButton(this);
	m_toggle_button->setCursor(Qt::PointingHandCursor);
	m_toggle_button->setStyleSheet("QPushButton { border: none; }");
	m_toggle_button->setFixedSize(32, 32);
	m_toggle_button->setFocusPolicy(Qt::NoFocus);
	connect(m_toggle_button, &QPushButton::clicked, [this]() {
		m_visible = !m_visible;
		setEchoMode(m_visible ? QLineEdit::Normal : QLineEdit::Password);
		updateToggleButtonIcon();
	});

	// 设置按钮的位置
	const int padding = 5; // 右边距
	int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	// 计算按钮位置
	QSize buttonSize = m_toggle_button->size();
	m_toggle_button->move(rect().right() - frameWidth - buttonSize.width() - padding,
						  (rect().height() - buttonSize.height()) / 2);

	updateToggleButtonIcon();
	setEchoMode(QLineEdit::Password);
}

void PasswdLineEdit::initLeadingAction() {
	// 设置 leading action
	if (m_leading_action == nullptr)
		m_leading_action = addAction(QIcon(m_leading_icon), QLineEdit::LeadingPosition);
}

void PasswdLineEdit::enterEvent(QEnterEvent* event) {
	if (m_with_toggle_button) {
		if (m_visible)
			m_toggle_button->setIcon(QIcon(m_visible_hover_icon));
		else
			m_toggle_button->setIcon(QIcon(m_invisible_hover_icon));
	}

	QLineEdit::enterEvent(event);
}

void PasswdLineEdit::leaveEvent(QEvent* event) {
	if (m_with_toggle_button) {
		updateToggleButtonIcon();
	}

	QLineEdit::leaveEvent(event);
}

void PasswdLineEdit::focusInEvent(QFocusEvent* event) {
	if (m_with_leading_action) {
		m_leading_action->setIcon(QIcon(m_leading_focus_icon));
	}
	// 父类处理节点
	QLineEdit::focusInEvent(event);
}

void PasswdLineEdit::focusOutEvent(QFocusEvent* event) {
	if (m_with_leading_action) {
		m_leading_action->setIcon(QIcon(m_leading_icon));
	}
	// 父类处理节点
	QLineEdit::focusOutEvent(event);
}
