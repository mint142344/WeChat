#include "custom/SearchListWidget.h"
#include "custom/MessageItem.h"

#include <QScrollBar>

SearchListWidget::SearchListWidget(QWidget* parent) : QListWidget{parent} {
	// 像素单位滚动
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	// 去掉 水平滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	verticalScrollBar()->hide();

	connect(this, &QListWidget::itemClicked, this, &SearchListWidget::onItemClicked);

	for (int i = 0; i < 20; ++i) {
		MessageItem* item = new MessageItem{this};
		item->setAvatar(QUrl(":/images/default_avatar.png"));
		item->setTitle("查找结果-" + QString::number(i));
		item->setMessage("来自: " + QString::number(i));
		item->setTimeVisible(false);
		item->setNotDisturbVisible(false);
		item->setGroup(i % 2 == 0);
		if (item->isGroup())
			item->setToolTip("群聊");
		else
			item->setToolTip("好友");

		addSearchItem(item);
	}
}

void SearchListWidget::addSearchItem(MessageItem* item) {
	QListWidgetItem* list_item = new QListWidgetItem(this);

	setItemWidget(list_item, item);
	list_item->setSizeHint(QSize(0, 64));

	addItem(list_item);
}

void SearchListWidget::enterEvent(QEnterEvent* event) { verticalScrollBar()->show(); }

void SearchListWidget::leaveEvent(QEvent* event) { verticalScrollBar()->hide(); }

void SearchListWidget::onItemClicked(QListWidgetItem* item) {
	MessageItem* msg_item = qobject_cast<MessageItem*>(itemWidget(item));
	if (msg_item == nullptr) {
		return;
	}

	// 显示聊天界面
	qInfo() << "SearchListWidget::onItemClicked: " << msg_item->getTitle();
}
