#include "DragWidgetFilter.h"
#include <qnamespace.h>

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

DragWidgetFilter::DragWidgetFilter(QObject *parent) : QObject(parent) {}

bool DragWidgetFilter::eventFilter(QObject *watched, QEvent *event) {
	QWidget *widget = qobject_cast<QWidget *>(watched);

	// 不过滤 继续传递事件
	if (widget == nullptr) return false;
	switch (event->type()) {
		case QEvent::MouseButtonPress: {
			QMouseEvent *mouse_ev = static_cast<QMouseEvent *>(event);
			if (mouse_ev->button() == Qt::LeftButton) {
				m_mouse_press = true;
				m_last_pos = mouse_ev->globalPosition();
			}
		} break;

		case QEvent::MouseButtonRelease: {
			QMouseEvent *mouse_ev = static_cast<QMouseEvent *>(event);
			if (mouse_ev->button() == Qt::LeftButton) {
				m_mouse_press = false;
			}
		} break;

		case QEvent::MouseMove: {
			QMouseEvent *mouse_ev = static_cast<QMouseEvent *>(event);
			if (m_mouse_press) {
				const QPointF delta = mouse_ev->globalPosition() - m_last_pos;
				widget->move(widget->x() + delta.x(), widget->y() + delta.y());
				m_last_pos = mouse_ev->globalPosition();
			}
		}

		break;
		default:
			return false;
	}

	return false;
}
