#pragma once

#include <QLineEdit>
#include <QKeyEvent>

// 搜索框
class SearchLineEdit : public QLineEdit {
	Q_OBJECT
public:
	explicit SearchLineEdit(QWidget* parent = nullptr) : QLineEdit(parent) {}

signals:
	void focusIn();
	void focusOut();

protected:
	void focusInEvent(QFocusEvent* event) override {
		QLineEdit::focusInEvent(event);
		emit focusIn();
	}

	void focusOutEvent(QFocusEvent* event) override {
		QLineEdit::focusOutEvent(event);
		emit focusOut();
	}

	void keyPressEvent(QKeyEvent* event) override {
		if (event->key() == Qt::Key_Escape) {
			clearFocus();
			emit focusOut();
		}
		QLineEdit::keyPressEvent(event);
	}
};