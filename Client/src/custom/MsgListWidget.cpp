#include "custom/MsgListWidget.h"
#include <qaction.h>
#include <QScrollBar>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>

MsgListWidget::MsgListWidget(QWidget* parent) : QListWidget{parent}, m_ctx_menu{new QMenu(this)} {
	initMenu();
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

void MsgListWidget::contextMenuEvent(QContextMenuEvent* event) {
	m_current_item = itemAt(event->pos());

	if (m_current_item == nullptr) {
		return;
	}
	MessageItem* message = dynamic_cast<MessageItem*>(itemWidget(m_current_item));
	if (message == nullptr) {
		return;
	}
	m_pin_top_act->setText(message->isPinTop() ? "取消置顶" : "置顶");
	if (message->isGroup()) {
		m_ctx_menu->addAction(m_quit_group_act);
	} else {
		m_ctx_menu->removeAction(m_quit_group_act);
	}

	m_ctx_menu->exec(event->globalPos());
}

void MsgListWidget::initMenu() {
	m_delete_msg_act = m_ctx_menu->addAction("从消息列表中删除");
	m_copy_title_act = m_ctx_menu->addAction("复制标题");
	m_pin_top_act = m_ctx_menu->addAction("置顶");
	m_quit_group_act = new QAction("退出群聊", this);

	connect(m_delete_msg_act, &QAction::triggered, this, &MsgListWidget::deleteMessage);
	connect(m_copy_title_act, &QAction::triggered, this, &MsgListWidget::copyTitle);
	connect(m_pin_top_act, &QAction::triggered, this, &MsgListWidget::pinTop);
	connect(m_quit_group_act, &QAction::triggered, this, &MsgListWidget::quitGroup);
}

void MsgListWidget::deleteMessage() {
	if (m_current_item == nullptr) {
		return;
	}
	delete takeItem(row(m_current_item));
	m_current_item = nullptr;
}

void MsgListWidget::copyTitle() {
	if (m_current_item == nullptr) {
		return;
	}
	MessageItem* item = dynamic_cast<MessageItem*>(itemWidget(m_current_item));
	if (item == nullptr) {
		return;
	}
	QApplication::clipboard()->setText(item->getTitle());
}

void MsgListWidget::pinTop() {
	if (m_current_item == nullptr) {
		return;
	}
	// 获取当前选中的 item
	MessageItem* message = dynamic_cast<MessageItem*>(itemWidget(m_current_item));
	if (message == nullptr) {
		return;
	}
	bool is_top = message->isPinTop();

	// NOTE: takeItem 会析构 itemWidget

	// 先 copy
	MessageItem* message_cp{};
	if (message != nullptr) {
		message_cp = message->clone();
	}

	// 再 takeItem
	QListWidgetItem* item = takeItem(row(m_current_item));
	if (is_top) {
		message_cp->setPinTop(false);
		insertItem(count(), item);
	} else {
		message_cp->setPinTop(true);
		insertItem(0, item);
	}
	setItemWidget(item, message_cp);

	m_current_item = nullptr;
}

void MsgListWidget::quitGroup() {
	if (m_current_item == nullptr) {
		return;
	}
	// TODO: 退出群聊
	delete takeItem(row(m_current_item));
	m_current_item = nullptr;
}
