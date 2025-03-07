#include "custom/GroupTreeItem.h"
#include <QHBoxLayout>

GroupTreeItem::GroupTreeItem(QWidget* parent)
	: QWidget{parent},
	  m_label_icon{new QLabel{this}},
	  m_label_text{new QLabel{this}},
	  m_label_cnt{new QLabel{"0", this}} {
	initLayout();
}

void GroupTreeItem::initLayout() {
	m_expand_icon = QPixmap(":/images/expand.png").scaled(16, 16);
	m_collapse_icon = QPixmap(":/images/collapse.png").scaled(16, 16);

	QHBoxLayout* layout = new QHBoxLayout{this};
	layout->setContentsMargins(8, 4, 8, 4);
	layout->setSpacing(10);

	m_label_icon->setPixmap(m_collapse_icon);
	m_label_icon->setFixedSize(16, 16);
	layout->addWidget(m_label_icon);

	m_label_text->setFont(QFont("Microsoft YaHei", 10));
	m_label_text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	layout->addWidget(m_label_text);
	layout->addStretch();

	layout->addWidget(m_label_cnt);
	layout->setAlignment(m_label_cnt, Qt::AlignRight | Qt::AlignVCenter);
	setLayout(layout);

	setFixedHeight(30);
}
