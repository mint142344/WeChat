#pragma once

#include <QStack>
#include <QMap>
#include <QWidget>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget {
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void switchTheme(const QString& path);

private:
	void initUi();
	void initSignalSlots();

private slots:
	void on_btn_avatar_clicked();
	void on_btn_setting_clicked();

	void on_btn_chat_toggled(bool checked);
	void on_btn_contact_toggled(bool checked);
	void on_btn_robot_toggled(bool checked);

	// 添加好友/群
	void addFriendOrGroup();
	// 创建群
	void createGroup();

	// 导航栏按钮组
	void buttonClicked(int id);

private:
	Ui::MainWindow* ui;
	QButtonGroup* m_btn_group;
	// 按钮id-isChecked
	QStack<QMap<int, bool>> m_id_checked;

	// 添加好友/群
	QMenu* m_add_menu;
};