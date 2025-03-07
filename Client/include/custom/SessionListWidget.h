#pragma once

#include "custom/SessionItem.h"

#include <QListWidget>
#include <QAction>

// robot 会话列表控件(时间排序)
class SessionListWidget : public QListWidget {
	Q_OBJECT
public:
	explicit SessionListWidget(QWidget* parent = nullptr);

	void addSessionItem(SessionItem* item);

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;

	// 右键菜单
	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	void initUi();

	// 初始化右键菜单
	void initMenu();

private slots:
	// 创建新会话
	void createNewSession();
	// 重命名会话
	void renameSession();
	// 删除会话
	void deleteSession();
	// 置顶会话
	void pinTopSession();

private:
	// 右键菜单
	QMenu* m_ctx_menu;
	QAction* m_new_session_act{};	 // 创建新会话
	QAction* m_rename_session_act{}; // 重命名会话
	QAction* m_delete_session_act{}; // 删除会话
	QAction* m_pin_top_act{};		 // 置顶/取消置顶

	// 当前右键选中的会话
	QListWidgetItem* m_current_item{};
};