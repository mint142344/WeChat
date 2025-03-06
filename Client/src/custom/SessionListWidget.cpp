#include "custom/SessionListWidget.h"
#include <QScrollBar>

SessionListWidget::SessionListWidget(QWidget* parent) : QListWidget{parent} {
	// 像素单位滚动
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	// 去掉 水平滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	verticalScrollBar()->hide();
}

void SessionListWidget::addSessionItem(SessionItem* item) {
	QListWidgetItem* list_item = new QListWidgetItem(this);

	list_item->setSizeHint(QSize(0, 50));
	setItemWidget(list_item, item);

	addItem(list_item);
}

void SessionListWidget::enterEvent(QEnterEvent* event) { verticalScrollBar()->show(); }

void SessionListWidget::leaveEvent(QEvent* event) { verticalScrollBar()->hide(); }
