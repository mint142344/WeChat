#include "custom/MessageItem.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>

MessageItem::MessageItem(QWidget* parent)
	: QWidget{parent},
	  is_read{false},
	  is_group{false},
	  is_disturb{false},
	  is_pin_top{false},
	  m_label_avatar{new QLabel{this}},
	  m_label_title{new QLabel{this}},
	  m_label_message{new QLabel{this}},
	  m_label_time{new QLabel{this}},
	  m_label_icon(new QLabel(this)) {
	initLayout();
}

void MessageItem::setNotDisturb(bool disturb, int cnt) {
	is_disturb = disturb;

	if (is_disturb) {
		// 免打扰
		auto pixmap = QPixmap(":/images/not_disturb.png")
						  .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		m_label_icon->setPixmap(pixmap);
		m_label_icon->show();

	} else {
		// 未读消息数
		m_label_icon->setFixedSize(24, 24);
		m_label_icon->setStyleSheet(
			"font-size: 10px; color: #FFFFFF; font-weight:bold; background-color: #494949; "
			"border-radius: 8px;");

		m_label_icon->setAlignment(Qt::AlignCenter);
		if (cnt <= 0) {
			m_label_icon->hide();
		} else {
			m_label_icon->show();
			if (cnt > 99)
				m_label_icon->setText("99+");
			else
				m_label_icon->setText(QString::number(cnt));
		}
	}
}

MessageItem* MessageItem::clone() {
	MessageItem* item = new MessageItem;
	item->setAvatar(m_label_avatar->pixmap());
	item->setTitle(m_label_title->text());
	item->setMessage(m_label_message->text());
	item->m_label_time->setText(m_label_time->text());
	item->setHasRead(is_read);
	item->setGroup(is_group);
	item->setNotDisturb(is_disturb);
	item->setPinTop(is_pin_top);

	return item;
}

void MessageItem::enterEvent(QEnterEvent* event) {
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor(0x393939));
	setPalette(pal);

	QWidget::enterEvent(event);
}

void MessageItem::leaveEvent(QEvent* event) {
	QPalette pal = palette();
	pal.setColor(QPalette::Window, Qt::transparent);
	setPalette(pal);

	QWidget::leaveEvent(event);
}

void MessageItem::initLayout() {
	// Main Layout
	QHBoxLayout* main_layout = new QHBoxLayout(this);
	main_layout->setContentsMargins(10, 8, 10, 8);
	main_layout->setSpacing(10);

	// 左-头像
	m_label_avatar->setFixedSize(45, 45);
	m_label_avatar->setScaledContents(true);
	main_layout->addWidget(m_label_avatar);

	// 右-垂直布局
	QVBoxLayout* right_layout = new QVBoxLayout;
	right_layout->setContentsMargins(0, 0, 0, 0);
	right_layout->setSpacing(3);

	// 右上 水平 - 标题 + 时间
	QHBoxLayout* top_layout = new QHBoxLayout;
	top_layout->setContentsMargins(0, 0, 0, 0);
	top_layout->setSpacing(5);

	// 标题
	m_label_title->setStyleSheet("font-size: 16px; color: #FFFFFF;");
	m_label_title->setFixedHeight(20);

	// 时间
	m_label_time->setStyleSheet("font-size: 11px; color: #B2B2B2;");
	m_label_time->setFixedHeight(20);
	top_layout->addWidget(m_label_title);
	top_layout->addStretch();
	top_layout->addWidget(m_label_time);
	right_layout->addLayout(top_layout);

	// 右下 - 消息 + 未读消息数/免打扰
	QHBoxLayout* bottom_layout = new QHBoxLayout;
	bottom_layout->setContentsMargins(0, 0, 0, 0);
	bottom_layout->setSpacing(5);

	// 消息
	m_label_message->setStyleSheet("font-size: 12px; color: #B2B2B2;");
	m_label_message->setFixedHeight(20);

	// 未读消息数 or 免打扰
	bottom_layout->addWidget(m_label_message);
	bottom_layout->addStretch();
	bottom_layout->addWidget(m_label_icon);
	right_layout->addLayout(bottom_layout);

	main_layout->addLayout(right_layout);
	setLayout(main_layout);
}
