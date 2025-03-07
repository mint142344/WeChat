#include "custom/SessionItem.h"
#include <QHBoxLayout>
#include <QPalette>

SessionItem::SessionItem(QWidget* parent)
	: QWidget{parent}, m_label_title{new QLabel{this}}, m_label_time{new QLabel{this}} {
	initLayout();
}

SessionItem* SessionItem::clone() {
	SessionItem* item = new SessionItem;
	item->m_label_title->setText(m_label_title->text());
	item->m_label_time->setText(m_label_time->text());
	item->m_uuid = m_uuid;
	item->m_pin_top = m_pin_top;

	return item;
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

	// 确保 时间 最右侧
	main_layout->addWidget(m_label_title, 1);
	main_layout->addWidget(m_label_time);
	setLayout(main_layout);
}
