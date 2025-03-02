#pragma once

#include "Singleton.hpp"
#include "Common.h"

#include <QTcpSocket>
#include <QMap>
#include <QTimer>
#include <QQueue>

// QObject 属性：id, token
class TcpClient : public QObject, public Singleton<TcpClient> {
	Q_OBJECT

	friend class Singleton<TcpClient>;

	using Callback = std::function<void(const QByteArray&)>;
	// 消息类型
	using Message = QPair<MsgID, QByteArray>;

	enum ReadState {
		HEADER, // 准备读取消息头
		BODY,	// 准备读取消息体
	};

public:
	// 发送消息到消息队列 id:消息ID body:消息体
	void sendMsg(MsgID id, const QByteArray& body);

	// 登录到ChatServer
	void loginToHost(const QString& host, uint16_t port);

private:
	// 构造
	TcpClient(QObject* parent = nullptr);

	// 注册回调
	void registerCallback();

	// 回调处理
	void handleMessage(MsgID id, const QByteArray& body);

	// 登录 ChatServer
	void loginChatServer();

	// 处理 发送队列
	void processSendQueue();

private slots:
	// 成功连接 ChatServer
	void onConnected();

	// 与 ChatServer 断开连接
	void onDisconnected();

	void onReadyRead();

	void onStateChanged(QAbstractSocket::SocketState socketState);

	void onError(QAbstractSocket::SocketError socketError);

	// 发送心跳包
	void sendHeartbeat();

signals:
	// 已登录 ChatServer
	void loggedChatServer(bool success, const QString& = "");

	// 与 ChatServer 断开连接
	void disconnectedChatServer();

	// 已发送消息
	void sentMsg(MsgID, qint64 length);

private:
	// 连接到 ChatServer 后,是否已认证
	bool m_auth = false;

	QTcpSocket* m_socket;
	// 心跳包 发送定时器
	QTimer* m_hearbeat_timer;

	// 消息包回调
	QMap<MsgID, Callback> m_callbacks;

	// 消息发送队列
	QQueue<Message> m_send_queue;
	// 消息暂存缓冲区
	QByteArray m_buffer;
	// 消息头
	MessageHeader<MsgID> m_header;
	// 读取状态
	ReadState m_state = ReadState::HEADER;
};