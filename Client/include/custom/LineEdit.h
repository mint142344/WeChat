#pragma once

#include <QLineEdit>
#include <QAction>

class LineEdit : public QLineEdit {
	Q_OBJECT
public:
	LineEdit(QWidget* parent = nullptr);
	~LineEdit() = default;

public:
	void setLeadingAction(QAction* action);
	void setLeadingFocusAction(QAction* action);

protected:
	void showEvent(QShowEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void focusOutEvent(QFocusEvent* event) override;

private:
	QAction* m_leading;
	QAction* m_leadingFocus;
};
