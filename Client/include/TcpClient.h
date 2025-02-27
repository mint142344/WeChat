#pragma once

#include "Singleton.hpp"
#include "Common.h"

#include <QTcpSocket>
#include <QMap>

// QObject 属性：id, token
class TcpClient : public QObject,
				  public Singleton<TcpClient>,
				  public std::enable_shared_from_this<TcpClient> {
	Q_OBJECT

	friend class Singleton<TcpClient>;

	using Callback = std::function<void(const QByteArray&)>;

	enum ReadState {
		HEADER, // 准备读取消息头
		BODY,	// 准备读取消息体
	};

public:
	// 发送消息 id:消息ID body:消息体
	void sendMsg(MsgID id, const QByteArray& body);
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

private slots:
	// 成功连接 ChatServer
	void onConnected();
	// 与 ChatServer 断开连接
	void onDisconnected();
	void onReadyRead();
	void onStateChanged(QAbstractSocket::SocketState socketState);
	void onError(QAbstractSocket::SocketError socketError);

signals:
	// 已登录 ChatServer
	void loggedChatServer(bool success, const QString& = "");

	// 与 ChatServer 断开连接
	void disconnectedChatServer();

	// 已发送消息
	void sentMsg(MsgID);

private:
	// 连接到 ChatServer 后,是否已认证
	bool m_auth = false;

	QTcpSocket* m_socket;

	QMap<MsgID, Callback> m_callbacks;

	QByteArray m_buffer;
	// 消息头
	message_header<MsgID> m_header;
	// 读取状态
	ReadState m_state = ReadState::HEADER;
};