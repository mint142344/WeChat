#pragma once

#include <boost/asio/steady_timer.hpp>

#include "net_common.h"
#include "net_message.hpp"
#include "net_server.hpp"

using namespace boost;

class ChatServer : public server_interface<ChatMsgType> {
	using ConnPtr = std::shared_ptr<connection<ChatMsgType>>;

public:
	ChatServer(uint16_t port);

private:
	bool onClientConnect(ConnPtr conn) override;

	void onClientDisconnect(ConnPtr conn) override;

	void onMessage(RcvMsgPtr<ChatMsgType> pMsg) override;

	void onServerStart() override;

private:
	void checkHeartBeat();

private:
	// 超时检测定时器
	asio::steady_timer m_beat_timer;

	size_t m_beat_timeout;
	size_t m_check_beat_time;
	size_t m_max_connections;
};