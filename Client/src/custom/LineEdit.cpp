#include "custom/LineEdit.h"

#include <QFocusEvent>

LineEdit::LineEdit(QWidget *parent)
	: QLineEdit(parent), m_leading(nullptr), m_leadingFocus(nullptr) {}

void LineEdit::setLeadingAction(QAction *action) { m_leading = action; }

void LineEdit::setLeadingFocusAction(QAction *action) { m_leadingFocus = action; }

void LineEdit::showEvent(QShowEvent *event) {
	assert(m_leading);
	addAction(m_leading, QLineEdit::ActionPosition::LeadingPosition);
}

void LineEdit::focusInEvent(QFocusEvent *event) {
	assert(m_leading);
	assert(m_leadingFocus);
	removeAction(m_leading);
	addAction(m_leadingFocus, QLineEdit::ActionPosition::LeadingPosition);
	QLineEdit::focusInEvent(event);
}

void LineEdit::focusOutEvent(QFocusEvent *event) {
	assert(m_leading);
	assert(m_leadingFocus);
	removeAction(m_leadingFocus);
	addAction(m_leading, QLineEdit::ActionPosition::LeadingPosition);
	QLineEdit::focusOutEvent(event);
}
