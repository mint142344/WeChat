#include "custom/ChatBubble.h"
#include <qwidget.h>
#include "Tool.h"

#include <QTextCursor>

ChatBubble::ChatBubble(QWidget* parent)

	: QWidget{parent},
	  m_label_avatar{new QLabel{this}},
	  m_label_username{new QLabel{this}},
	  m_label_timestamp{new QLabel{this}},
	  m_message_edit{new QTextEdit{this}},
	  m_direction{MessageDirection::In},
	  m_message_type{MessageType::Text},
	  m_max_width{400},
	  m_streamed{false},
	  m_stream_timer{new QTimer{this}},
	  m_stream_index{0} {
	initLayout();
}

void ChatBubble::setMessage(const QString& message) {
	m_full_message = message;

	if (!isStreamed()) {
		m_current_message = m_full_message;
		m_message_edit->setText(m_current_message);

	} else {
		m_current_message.clear();
		m_stream_index = 0;
	}

	updateLayout();
}

void ChatBubble::appendMessage(const QString& message) {
	m_full_message += message;

	if (!isStreamed()) {
		m_current_message = m_full_message;
		m_message_edit->setText(m_current_message);
		updateLayout();
	}

	// 流式由定时器完成追加
}

void ChatBubble::setAvatar(const QPixmap& avatar) {
	m_label_avatar->setPixmap(Tool::roundImage(avatar).scaled(32, 32));
}

void ChatBubble::setMessageDirection(MessageDirection direction, MessageType type) {
	m_message_type = type;
	m_direction = direction;
	updateLayout();

	// 设置气泡颜色
	if (direction == MessageDirection::Out) {
		m_message_edit->setStyleSheet(
			"background-color: #95ec69; border-radius: 8px; padding: 8px;");
	} else {
		m_message_edit->setStyleSheet(
			"background-color: #ffffff; border-radius: 8px; padding: 8px;");
	}
}

void ChatBubble::setMaxWidth(int maxWidth) {
	m_max_width = maxWidth;
	m_message_edit->setFixedWidth(m_max_width - 80);
	updateLayout();
}

void ChatBubble::startStreaming() {
	// 每 15ms 更新一次
	if (isStreamed() && !m_stream_timer->isActive()) {
		m_current_message.clear();
		m_stream_index = 0;
		m_stream_timer->start(15);
	}
}

void ChatBubble::stopStreaming() {
	m_stream_timer->stop();
	// 确保显示完整消息
	m_current_message = m_full_message;
	m_message_edit->setText(m_current_message);
	updateLayout();
}

void ChatBubble::paintEvent(QPaintEvent* event) {
	QWidget::paintEvent(event);
	// 气泡尾巴
}

void ChatBubble::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	updateLayout();
}

void ChatBubble::initLayout() {
	// 头像
	m_label_avatar->setFixedSize(32, 32);
	m_label_avatar->setScaledContents(true);

	// 用户名
	m_label_username->setFont(QFont("Microsoft YaHei", 9));

	// 消息编辑框
	m_message_edit->setReadOnly(true);
	m_message_edit->setFrameShape(QFrame::NoFrame);
	m_message_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_message_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_message_edit->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
	m_message_edit->setTextInteractionFlags(Qt::TextSelectableByMouse |
											Qt::TextSelectableByKeyboard);
	// 时间戳
	m_label_timestamp->setFont(QFont("Microsoft YaHei", 8));
	m_label_timestamp->setAlignment(Qt::AlignRight | Qt::AlignBottom);
	m_label_timestamp->setStyleSheet("color: #808080;");

	// 更新布局
	updateLayout();

	// 流式显示定时器
	connect(m_stream_timer, &QTimer::timeout, [this] {
		if (m_stream_index < m_full_message.length()) {
			// 追加
			m_current_message = m_full_message.left(m_stream_index++);
			m_message_edit->setText(m_current_message);

			// 滚动到底部
			QTextCursor cursor = m_message_edit->textCursor();
			cursor.movePosition(QTextCursor::End);
			m_message_edit->setTextCursor(cursor);

			// 更新布局
			updateLayout();

		} else {
			stopStreaming();
		}
	});
}

void ChatBubble::updateLayout() {}

QSize ChatBubble::calculateBubbleSize() const {
	int text_width = m_message_edit->document()->size().width();
	int text_height = m_message_edit->document()->size().height();

	text_width = qMin(text_width, m_max_width - 80);

	return QSize(text_width + 80, text_height + 60);
}
