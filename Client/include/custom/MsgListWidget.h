#pragma once

#include "custom/MessageItem.h"
#include <QListWidget>
#include <QAction>

// 消息列表控件
class MsgListWidget : public QListWidget {
	Q_OBJECT
public:
	explicit MsgListWidget(QWidget* parent = nullptr);

	void addMessageItem(MessageItem* item);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	// 右键菜单
	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	// 初始化右键菜单
	void initMenu();

private slots:
	// 从消息列表中删除
	void deleteMessage();
	// 复制标题 到 剪贴板
	void copyTitle();
	// 置顶/取消置顶
	void pinTop();
	// 退出群聊
	void quitGroup();

private:
	// 右键菜单
	QMenu* m_ctx_menu;
	QAction* m_delete_msg_act{}; // 从消息列表中删除
	QAction* m_copy_title_act{}; // 复制标题
	QAction* m_pin_top_act{};	 // 置顶/取消置顶
	QAction* m_quit_group_act{}; // 退出群聊

	// 当前右键选中
	QListWidgetItem* m_current_item{};
};