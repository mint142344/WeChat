#pragma once
#include <qtreewidget.h>
#include "custom/ContactTreeItem.h"

#include <QTreeWidget>

// 好友/群聊 列表
class ContactTreeWidget : public QTreeWidget {
	Q_OBJECT
public:
	explicit ContactTreeWidget(QWidget* parent = nullptr);

	// 获取所有分组
	QStringList getAllGroups() const { return m_groups.keys(); }

	// 添加item to group
	void addItemToGroup(const QString& group, ContactTreeItem* item);

	// 添加分组
	void addGroup(const QString& group);

	// void loadFriendsFromJson(const QJsonArray& friends);
	// void loadGroupsFromJson(const QJsonArray& groups);

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
	// 分组操作：添加分组
	void addGroupByMenu();
	// 分组操作：删除分组
	void removeGroup();
	// 分组操作：重命名分组
	void renameGroup();

	// item操作：移动 item 到 分组
	void moveContactItemToGroup(const QString& group);
	// item操作：删除 item
	void removeContactItem();
	// item操作：添加 item
	void addItem(const QString& group, ContactTreeItem* item);

private:
	void initMenu();

	void updateMenu();

private:
	// 所有分组
	QMap<QString, QTreeWidgetItem*> m_groups;

	QMenu* m_group_menu;		   // Group操作 菜单
	QAction* m_add_group_act{};	   // 添加分组
	QAction* m_del_group_act{};	   // 删除分组
	QAction* m_rename_group_act{}; // 重命名分组

	QMenu* m_contact_menu;		  // Item操作 菜单
	QMenu* m_move_menu{};		  // 移动 item 到 分组
	QAction* m_del_contact_act{}; // 删除 item
	QAction* m_send_act{};		  // 发送消息

	QTreeWidgetItem* m_cur_item{}; // selected  item
};