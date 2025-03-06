#include "custom/SessionItem.h"
#include <QHBoxLayout>

SessionItem::SessionItem(QWidget* parent)
	: QWidget{parent}, m_label_title{new QLabel{this}}, m_label_time{new QLabel{this}} {
	initLayout();
}

void SessionItem::enterEvent(QEnterEvent* event) {
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor(0x393939));
	setPalette(pal);

	QWidget::enterEvent(event);
}

void SessionItem::leaveEvent(QEvent* event) {
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::transparent);
	setPalette(pal);

	QWidget::leaveEvent(event);
}

void SessionItem::initLayout() {
	QHBoxLayout* main_layout = new QHBoxLayout{this};
	main_layout->setContentsMargins(10, 5, 10, 5);
	main_layout->setSpacing(10);

	// 标题
	m_label_title->setStyleSheet("font-size: 16px; color: #FFFFFF;");
	m_label_title->setFixedHeight(20);

	// 时间
	m_label_time->setStyleSheet("font-size: 11px; color: #B2B2B2;");
	m_label_time->setFixedHeight(20);

	main_layout->addWidget(m_label_title);
	main_layout->addWidget(m_label_time);
	setLayout(main_layout);
}
