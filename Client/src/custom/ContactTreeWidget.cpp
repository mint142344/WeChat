#include "custom/ContactTreeWidget.h"
#include "custom/ContactTreeItem.h"
#include "custom/GroupTreeItem.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>

ContactTreeWidget::ContactTreeWidget(QWidget* parent)
	: QTreeWidget(parent), m_group_menu(new QMenu(this)), m_contact_menu(new QMenu(this)) {
	setIndentation(8); // 缩进
	setRootIsDecorated(false);
	setHeaderHidden(true);									  // 隐藏表头
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel); // 平滑滚动
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);	  // 隐藏水平滚动条
	// setAnimated(true);										  // 动画效果 会卡

	// 设置 top-level item 的列数
	setColumnCount(1);
	initMenu();
}

void ContactTreeWidget::addItemToGroup(const QString& group, ContactTreeItem* item) {
	if (m_groups.contains(group)) {
		QTreeWidgetItem* top_item = m_groups.value(group);
		QTreeWidgetItem* child_item = new QTreeWidgetItem(top_item);
		setItemWidget(child_item, 0, item);

	} else {
		QTreeWidgetItem* top_item = new QTreeWidgetItem(this);

		GroupTreeItem* group_item = new GroupTreeItem(this);
		group_item->setGroupName(group);
		group_item->setGroupSize(0);
		setItemWidget(top_item, 0, group_item);

		connect(group_item, &GroupTreeItem::toggled, [this, top_item]() {
			blockSignals(true);
			top_item->setExpanded(!top_item->isExpanded());
			blockSignals(false);
		});

		top_item->setExpanded(group_item->isExpanded());
		m_groups.insert(group, top_item);
		addTopLevelItem(top_item);

		QTreeWidgetItem* child_item = new QTreeWidgetItem(top_item);
		setItemWidget(child_item, 0, item);
	}

	// 更新分组 人数
	GroupTreeItem* group_item = dynamic_cast<GroupTreeItem*>(itemWidget(m_groups.value(group), 0));
	group_item->setGroupSize(group_item->getGroupSize() + 1);

	updateMenu();
}

void ContactTreeWidget::addGroup(const QString& group) {
	if (m_groups.contains(group)) {
		return;
	}

	QTreeWidgetItem* top_item = new QTreeWidgetItem(this);

	GroupTreeItem* group_item = new GroupTreeItem(this);
	group_item->setGroupName(group);
	group_item->setGroupSize(0);
	setItemWidget(top_item, 0, group_item);

	connect(group_item, &GroupTreeItem::toggled, [this, top_item]() {
		blockSignals(true);
		top_item->setExpanded(!top_item->isExpanded());
		blockSignals(false);
	});

	top_item->setExpanded(group_item->isExpanded());
	m_groups.insert(group, top_item);
	addTopLevelItem(top_item);
	updateMenu();
}

void ContactTreeWidget::contextMenuEvent(QContextMenuEvent* event) {
	QTreeWidgetItem* item = itemAt(event->pos());

	// 空白区域
	if (item == nullptr) {
		qDebug() << topLevelItemCount();
		m_del_group_act->setVisible(false);
		m_rename_group_act->setVisible(false);
		m_group_menu->exec(event->globalPos());
		return;
	}

	m_del_group_act->setVisible(true);
	m_rename_group_act->setVisible(true);
	m_cur_item = item;

	// top-level item
	if (item->parent() == nullptr) {
		m_group_menu->exec(event->globalPos());
	} else {
		ContactTreeItem* widget = static_cast<ContactTreeItem*>(itemWidget(item, 0));
		if (widget->getType() == ContactTreeItem::Type::Friend) {
			m_send_act->setVisible(true);
		} else {
			m_send_act->setVisible(false);
		}
		m_contact_menu->exec(event->globalPos());
	}
}

void ContactTreeWidget::addGroupByMenu() {
	bool ok{};
	QString group = QInputDialog::getText(this, "添加分组", "分组名称", QLineEdit::Normal, "", &ok);
	if (m_groups.contains(group)) {
		QMessageBox::warning(this, "添加分组", "分组已存在");
		return;
	}

	if (ok && !group.isEmpty()) {
		QTreeWidgetItem* top_item = new QTreeWidgetItem(this);

		GroupTreeItem* group_item = new GroupTreeItem(this);
		group_item->setGroupName(group);
		group_item->setGroupSize(0);
		setItemWidget(top_item, 0, group_item);

		connect(group_item, &GroupTreeItem::toggled, [this, top_item]() {
			blockSignals(true);
			top_item->setExpanded(!top_item->isExpanded());
			blockSignals(false);
		});

		top_item->setExpanded(group_item->isExpanded());
		m_groups.insert(group, top_item);
		addTopLevelItem(top_item);
		updateMenu();
	}
}

void ContactTreeWidget::removeGroup() {
	if (m_cur_item != nullptr) {
		takeTopLevelItem(indexOfTopLevelItem(m_cur_item));

		m_groups.remove(m_cur_item->text(0));
		m_cur_item = nullptr;
		updateMenu();
	}
}

void ContactTreeWidget::renameGroup() {
	if (m_cur_item == nullptr) return;

	bool ok{};
	QString group = QInputDialog::getText(this, "重命名分组", "分组名称", QLineEdit::Normal,
										  m_cur_item->text(0), &ok);

	if (ok && !group.isEmpty()) {
		m_cur_item->setText(0, group);

		m_groups.remove(m_cur_item->text(0));
		m_groups.insert(group, m_cur_item);
		updateMenu();
	}
}

void ContactTreeWidget::moveContactItemToGroup(const QString& group) {}

void ContactTreeWidget::removeContactItem() {}

void ContactTreeWidget::addItem(const QString& group, ContactTreeItem* item) {}

void ContactTreeWidget::initMenu() {
	setContextMenuPolicy(Qt::DefaultContextMenu);
	// 分组操作 菜单
	m_add_group_act = m_group_menu->addAction("添加分组");
	m_del_group_act = m_group_menu->addAction("删除分组");
	m_rename_group_act = m_group_menu->addAction("重命名分组");

	connect(m_add_group_act, &QAction::triggered, this, &ContactTreeWidget::addGroupByMenu);
	connect(m_del_group_act, &QAction::triggered, this, &ContactTreeWidget::removeGroup);
	connect(m_rename_group_act, &QAction::triggered, this, &ContactTreeWidget::renameGroup);

	// item操作 菜单
	m_send_act = m_contact_menu->addAction("发送消息");
	m_del_contact_act = m_contact_menu->addAction("删除");
	m_move_menu = m_contact_menu->addMenu("移动到");

	connect(m_del_contact_act, &QAction::triggered, this, &ContactTreeWidget::removeContactItem);
}

void ContactTreeWidget::updateMenu() {
	m_move_menu->clear();
	for (int i = 0; i < m_groups.size(); ++i) {
		QString group = m_groups.keys().at(i);
		m_move_menu->addAction(group, this, [this, group]() {
			qInfo() << "move to " << group;
			// 移动 selected item 到 group
			moveContactItemToGroup(group);
		});
	}
}
