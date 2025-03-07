#include "custom/ContactTreeItem.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

ContactTreeItem::ContactTreeItem(QWidget* parent)
	: QWidget{parent},
	  m_label_avatar{new QLabel{this}},
	  m_label_title{new QLabel{this}},
	  m_label_message{new QLabel{this}} {
	initLayout();
}

void ContactTreeItem::initLayout() {
	QHBoxLayout* main_layout = new QHBoxLayout{this};
	main_layout->setContentsMargins(10, 10, 10, 10);
	main_layout->setSpacing(10);

	m_label_avatar->setFixedSize(40, 40);
	m_label_avatar->setScaledContents(true);
	main_layout->addWidget(m_label_avatar);

	QVBoxLayout* right_layout = new QVBoxLayout{};
	right_layout->setContentsMargins(0, 0, 0, 0);
	right_layout->setSpacing(5);

	m_label_title->setStyleSheet("font-size: 14px; color: #FFFFFF; font-weight: bold;");
	m_label_message->setStyleSheet("font-size: 12px; color:rgb(192, 192, 192);");
	right_layout->addWidget(m_label_title);
	right_layout->addWidget(m_label_message);
	main_layout->addLayout(right_layout);

	setFixedHeight(60);
	setLayout(main_layout);
}
