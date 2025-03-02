#pragma once

#include <boost/asio/steady_timer.hpp>

#include "net.h"
#include "net_message.hpp"
#include "net_server.hpp"

class ChatServer : public server_interface<MsgID> {
	using ConnPtr = std::shared_ptr<connection<MsgID>>;

public:
	ChatServer(uint16_t port);

private:
	bool onClientConnect(ConnPtr conn) override;

	void onClientDisconnect(ConnPtr conn) override;

	void onMessage(RcvMsgPtr<MsgID> pMsg) override;

	void onServerStart() override;

private:
	// 开启 超时心跳检测
	void checkHeartBeat();

	// 验证登录
	void verifyLogin(ConnPtr conn, RcvMsgPtr<MsgID> pMsg);

private:
	// 超时检测定时器
	net::steady_timer m_beat_timer;

	// 心跳检测 超时时间
	size_t m_beat_timeout;
	// 心跳检测 周期
	size_t m_beat_duration;
	size_t m_max_connections;
};