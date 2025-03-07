#pragma once

#include <qobject.h>
#include <qtreewidget.h>
#include <qwidget.h>
#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

// TreeWidget top-level-item(icon + text + cnt/size)
class GroupTreeItem : public QWidget {
	Q_OBJECT

public:
	explicit GroupTreeItem(QWidget* parent = nullptr);

	void setGroupName(const QString& name) { m_label_text->setText(name); }

	QString getGroupName() { return m_label_text->text(); }

	void setGroupSize(int cnt) { m_label_cnt->setText(QString("%1").arg(cnt)); }

	int getGroupSize() { return m_label_cnt->text().toInt(); }

	bool isExpanded() const { return is_expanded; }

protected:
	void mousePressEvent(QMouseEvent* event) override {
		if (event->button() == Qt::LeftButton) {
			m_label_icon->setPixmap(is_expanded ? m_collapse_icon : m_expand_icon);
			is_expanded = !is_expanded;
			emit toggled(is_expanded);
		}

		QWidget::mousePressEvent(event);
	}
signals:
	void toggled(bool is_expanded);

private:
	void initLayout();

private:
	QLabel* m_label_icon;
	QLabel* m_label_text;
	QLabel* m_label_cnt;

	bool is_expanded = false;
	QPixmap m_expand_icon;
	QPixmap m_collapse_icon;
};