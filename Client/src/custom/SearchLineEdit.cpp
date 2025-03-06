#include "custom/SearchLineEdit.h"

SearchLineEdit::SearchLineEdit(QWidget* parent) : QLineEdit{parent} {}

void SearchLineEdit::setSearchIcon(const QIcon& icon) {
	if (m_search_action != nullptr) {
		removeAction(m_search_action);
	}

	addAction(icon, QLineEdit::LeadingPosition);
}

void SearchLineEdit::setClearIcon(const QIcon& icon) {
	if (m_clear_action != nullptr) {
		removeAction(m_clear_action);
	}

	m_clear_action = addAction(icon, QLineEdit::TrailingPosition);
	m_clear_action->setVisible(false);
	connect(m_clear_action, &QAction::triggered, this, &QLineEdit::clear);
}

void SearchLineEdit::focusInEvent(QFocusEvent* event) {
	m_clear_action->setVisible(true);

	QLineEdit::focusInEvent(event);
}

void SearchLineEdit::focusOutEvent(QFocusEvent* event) {
	m_clear_action->setVisible(false);
	QLineEdit::focusOutEvent(event);
}
