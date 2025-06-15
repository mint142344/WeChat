#pragma once

#include <QWidget>
#include <QPixmap>
#include <QDateTime>
#include <QTimer>
#include <QLabel>
#include <QTextEdit>

// 消息类型枚举
enum class MessageType {
	Text,  // 文本
	Image, // 图片
	File,  // 文件
	Voice, // 语音
	Video, // 视频
};

// 消息方向枚举
enum class MessageDirection {
	Out, // 发送
	In	 // 接收
};

class ChatBubble : public QWidget {
	Q_OBJECT
public:
	explicit ChatBubble(QWidget* parent = nullptr);

	// 设置消息内容
	void setMessage(const QString& message);

	// 追加消息内容(流式显示)
	void appendMessage(const QString& message);

	// 设置头像
	void setAvatar(const QPixmap& avatar);

	// 设置消息方向与类型
	void setMessageDirection(MessageDirection direction, MessageType type);

	// 设置消息时间
	void setTimestamp(const QDateTime& timestamp) {
		m_label_timestamp->setText(timestamp.toString("HH:mm"));
	}

	// 设置用户名
	void setUsername(const QString& username) { m_label_username->setText(username); }

	// 设置最大宽度
	int maxWidth() const { return m_max_width; }
	void setMaxWidth(int maxWidth);

	// 启用流式显示
	void setStreamed(bool streamed) { m_streamed = streamed; }
	bool isStreamed() const { return m_streamed; }

	// 开始流式显示
	void startStreaming();

	// 停止流式显示
	void stopStreaming();

protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

private:
	// 初始化布局
	void initLayout();

	// 更新布局
	void updateLayout();

	// 计算气泡大小
	QSize calculateBubbleSize() const;

private:
	QLabel* m_label_avatar;	   // 头像
	QLabel* m_label_username;  // 用户名
	QLabel* m_label_timestamp; // 时间戳
	QTextEdit* m_text_message; // 消息内容

	MessageDirection m_direction; // 消息方向
	MessageType m_message_type;	  // 消息类型
	QString m_full_message;		  // 完整消息内容
	QString m_current_message;	  // 当前消息内容
	int m_max_width;			  // 最大宽度

	bool m_streamed;		// 是否流式显示
	QTimer* m_stream_timer; // 流式显示定时器
	int m_stream_index;		// 流式显示索引
};
