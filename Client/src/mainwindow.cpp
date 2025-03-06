#include "mainwindow.h"
#include "ChatWidget/NavigationBar.h"
#include "ChatWidget/SiderBarWidget.h"

#include <qtextedit.h>
#include <qtextformat.h>

#include <QApplication>
#include <QScreen>
#include <QSplitter>
#include <QHBoxLayout>
#include <QFile>

MainWindow::MainWindow(QWidget* parent)
	: QWidget(parent),
	  m_navigation_bar(new NavigationBar(this)),
	  m_main_layout(new QHBoxLayout(this)),
	  m_sider_bar(new SiderBarWidget(this)) {
	initLayout();
	initSignalSlots();
}

void MainWindow::initLayout() {
	// DPI
	qreal DPI = QApplication::primaryScreen()->devicePixelRatio();
	// 设置窗口大小
	int min_width = 1200 / DPI;
	int min_height = 800 / DPI;
	setMinimumSize(min_width, min_height);

	int default_width = 1600 / DPI;
	int default_height = 1000 / DPI;
	resize(default_width, default_height);

	// 主布局
	setContentsMargins(0, 0, 0, 0);
	m_main_layout->setContentsMargins(0, 0, 0, 0);
	m_main_layout->setSpacing(0);

	// 1.添加导航栏到最左侧
	m_main_layout->addWidget(m_navigation_bar);

	QSplitter* splitter = new QSplitter(Qt::Horizontal);
	// 设置分割线宽度
	splitter->setHandleWidth(0);

	// 2.侧边栏
	m_sider_bar->setMinMaxWidth(200, 300);

	QTextEdit* text_edit = new QTextEdit;

	splitter->addWidget(m_sider_bar);
	splitter->addWidget(text_edit);
	splitter->setStretchFactor(0, 0); // 0:不自动拉伸 只能手动拉伸
	splitter->setStretchFactor(1, 3);
	splitter->setCollapsible(0, false); // 禁止折叠

	m_main_layout->addWidget(splitter);

	// 使用默认主题
	switchTheme(":/style/themes/default.qss");
}

void MainWindow::initSignalSlots() {
	// 关联导航栏
	connect(m_navigation_bar, &NavigationBar::sigAvatarClicked,
			[this] { qDebug() << "avatar clicked"; });

	connect(m_navigation_bar, &NavigationBar::sigMsgClicked, [this] {
		qDebug() << "msg clicked";
		m_sider_bar->switchToView(SiderBarWidget::MessageView);
	});

	connect(m_navigation_bar, &NavigationBar::sigContactClicked, [this] {
		qDebug() << "contact clicked";
		m_sider_bar->switchToView(SiderBarWidget::ContactView);
	});

	connect(m_navigation_bar, &NavigationBar::sigRobotClicked, [this] {
		qDebug() << "robot clicked";
		m_sider_bar->switchToView(SiderBarWidget::RobotView);
	});

	connect(m_navigation_bar, &NavigationBar::sigSettingClicked,
			[this] { qDebug() << "setting clicked"; });

	// 关联侧边栏
	connect(m_sider_bar, &SiderBarWidget::addButtonClicked,
			[this] { qDebug() << "add button clicked"; });

	connect(m_sider_bar, &SiderBarWidget::searchTextChanged,
			[this](const QString& text) { qDebug() << "search text changed: " << text; });
}

void MainWindow::switchTheme(const QString& themePath) {
	QFile file(themePath);
	if (file.open(QFile::ReadOnly)) {
		QString styleSheet = QLatin1String(file.readAll());
		setStyleSheet(styleSheet);
		m_navigation_bar->setStyleSheet(styleSheet);
		m_sider_bar->setStyleSheet(styleSheet);

		file.close();
	}
}
