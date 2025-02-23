#pragma once

#include <QObject>
#include <QPoint>

class DragWidgetFilter : public QObject {
	Q_OBJECT
public:
	explicit DragWidgetFilter(QObject* parent = nullptr);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private:
	bool m_mouse_press = false;
	QPointF m_last_pos;
};