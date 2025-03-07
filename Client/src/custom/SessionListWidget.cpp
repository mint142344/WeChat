#include "custom/SessionListWidget.h"
#include "custom/SessionItem.h"

#include <QScrollBar>
#include <QMenu>
#include <QContextMenuEvent>
#include <QInputDialog>

SessionListWidget::SessionListWidget(QWidget* parent)
	: QListWidget{parent}, m_ctx_menu{new QMenu(this)} {
	initUi();
	initMenu();
}

void SessionListWidget::addSessionItem(SessionItem* item) {
	QListWidgetItem* list_item = new QListWidgetItem(this);

	list_item->setSizeHint(QSize(0, 50));
	// 设置 item 的 widget
	setItemWidget(list_item, item);

	addItem(list_item);
}

void SessionListWidget::enterEvent(QEnterEvent* event) { verticalScrollBar()->show(); }

void SessionListWidget::leaveEvent(QEvent* event) { verticalScrollBar()->hide(); }

void SessionListWidget::contextMenuEvent(QContextMenuEvent* event) {
	m_current_item = itemAt(event->pos());

	if (m_current_item == nullptr) {
		m_delete_session_act->setVisible(false);
		m_rename_session_act->setVisible(false);
		m_pin_top_act->setVisible(false);
	} else {
		SessionItem* session = dynamic_cast<SessionItem*>(itemWidget(m_current_item));
		if (session != nullptr) {
			m_pin_top_act->setText(session->isPinnedTop() ? "取消置顶" : "置顶会话");
		}

		m_delete_session_act->setVisible(true);
		m_rename_session_act->setVisible(true);
		m_pin_top_act->setVisible(true);
	}

	// 显示右键菜单
	m_ctx_menu->exec(event->globalPos());
}

void SessionListWidget::initUi() {
	// TODO:设置 frame
	setFrameShape(QFrame::NoFrame);
	// 像素单位滚动
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	// 去掉 水平滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	verticalScrollBar()->hide();
}

void SessionListWidget::initMenu() {
	setContextMenuPolicy(Qt::DefaultContextMenu);
	m_new_session_act = m_ctx_menu->addAction("新建会话");
	m_pin_top_act = m_ctx_menu->addAction("置顶会话");
	m_ctx_menu->addSeparator();
	m_rename_session_act = m_ctx_menu->addAction("重命名");
	m_delete_session_act = m_ctx_menu->addAction("删除会话");

	connect(m_new_session_act, &QAction::triggered, this, &SessionListWidget::createNewSession);
	connect(m_pin_top_act, &QAction::triggered, this, &SessionListWidget::pinTopSession);
	connect(m_rename_session_act, &QAction::triggered, this, &SessionListWidget::renameSession);
	connect(m_delete_session_act, &QAction::triggered, this, &SessionListWidget::deleteSession);
}

void SessionListWidget::createNewSession() {
	SessionItem* item = new SessionItem(this);
	item->setTitle("新会话");
	item->setTime(QDateTime::currentDateTime());

	addSessionItem(item);
}

void SessionListWidget::renameSession() {
	if (m_current_item == nullptr) {
		return;
	}
	SessionItem* item = dynamic_cast<SessionItem*>(itemWidget(m_current_item));
	QString old_title = item->getTitle();

	bool ok{};
	QString title = QInputDialog::getText(this, "重命名会话", "请输入新的会话名称",
										  QLineEdit::Normal, old_title, &ok);
	if (ok && !title.isEmpty() && title != old_title) {
		item->setTitle(title);
	}
}

void SessionListWidget::deleteSession() {
	if (m_current_item == nullptr) {
		return;
	}
	delete takeItem(row(m_current_item));
	m_current_item = nullptr;
}

void SessionListWidget::pinTopSession() {
	if (m_current_item == nullptr) {
		return;
	}
	SessionItem* session = dynamic_cast<SessionItem*>(itemWidget(m_current_item));
	if (session == nullptr) {
		return;
	}
	bool is_top = session->isPinnedTop();

	// NOTE: takeItem 会析构 itemWidget

	// 先 copy
	SessionItem* session_cp{};
	if (session != nullptr) {
		session_cp = session->clone();
	}

	// 再 takeItem
	QListWidgetItem* item = takeItem(row(m_current_item));

	// 重新设置 item  widget
	if (is_top) {
		session_cp->setPinTop(false);
		insertItem(count(), item);
	} else {
		session_cp->setPinTop(true);
		insertItem(0, item);
	}
	setItemWidget(item, session_cp);

	m_current_item = nullptr;
}
