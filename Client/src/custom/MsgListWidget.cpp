#include "custom/MsgListWidget.h"
#include <QScrollBar>

MsgListWidget::MsgListWidget(QWidget* parent) : QListWidget{parent} {
	// 像素单位滚动
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	// 去掉 水平滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	verticalScrollBar()->hide();
}

void MsgListWidget::addMessageItem(MessageItem* item) {
	QListWidgetItem* list_item = new QListWidgetItem(this);

	setItemWidget(list_item, item);
	list_item->setSizeHint(QSize(0, 64));

	addItem(list_item);
}

void MsgListWidget::enterEvent(QEnterEvent* event) { verticalScrollBar()->show(); }

void MsgListWidget::leaveEvent(QEvent* event) { verticalScrollBar()->hide(); }
