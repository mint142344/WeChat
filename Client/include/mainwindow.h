#pragma once

#include <QWidget>

class QHBoxLayout;
class NavigationBar;
class SiderBarWidget;

// 聊天主窗口
class MainWindow : public QWidget {
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);

private:
	// 初始化控件布局
	void initLayout();
	// 初始化槽函数
	void initSignalSlots();

private slots:
	// TODO: 切换主题
	void switchTheme(const QString& themePath);

private:
	// 主布局
	QHBoxLayout* m_main_layout;
	// 导航栏
	NavigationBar* m_navigation_bar;
	// 侧边栏
	SiderBarWidget* m_sider_bar;
};
