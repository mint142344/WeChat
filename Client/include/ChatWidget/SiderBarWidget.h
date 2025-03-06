#pragma once

#include "custom/SearchLineEdit.h"
#include "custom/MsgListWidget.h"
#include "custom/SessionListWidget.h"

#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QToolButton>

class SiderBarWidget : public QWidget {
	Q_OBJECT
public:
	enum ViewType {
		MessageView,
		ContactView,
		RobotView,
	};

public:
	explicit SiderBarWidget(QWidget* parent = nullptr);

	// 切换视图
	void switchToView(ViewType type);

	// 设置最小最大宽度
	void setMinMaxWidth(int min, int max);

signals:
	// 添加好友 按钮点击
	void addButtonClicked();

	// 搜索框 文本变化
	void searchTextChanged(const QString& text);

	// 消息项 点击
	void messageItemClicked(int index);

	// 联系人项 点击
	void contactItemClicked(QTreeWidgetItem* item, int column);

	// 机器人会话 点击
	void robotSessionClicked(int index);

private:
	// 初始化控件布局
	void initLayout();
	// 初始化信号槽
	void initSignalSlots();

private:
	QVBoxLayout* m_layout;

	// 搜索框
	SearchLineEdit* m_search_edit;
	QToolButton* m_btn_add;

	// 三种视图
	ViewType m_view_cur;
	MsgListWidget* m_view_msg;		 // 消息列表
	QTreeWidget* m_view_contact;	 // 联系人列表
	SessionListWidget* m_view_robot; // 机器人会话列表
};