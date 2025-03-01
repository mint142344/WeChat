#include "ChatServer.h"
#include "ConfigManager.h"

#include <fmt/base.h>
#include <chrono>

ChatServer::ChatServer(uint16_t port)
	: server_interface<ChatMsgType>(port),
	  m_beat_timer(m_ctx),
	  m_beat_timeout(ConfigManager::getInstance()->beatTimeout()),
	  m_check_beat_time(ConfigManager::getInstance()->checkBeatTime()),
	  m_max_connections(ConfigManager::getInstance()->maxConnections()) {}

// 客户端连接时 调用
bool ChatServer::onClientConnect(ConnPtr conn) {
	// 超过限制，拒绝新连接
	if (m_connections.size() >= m_max_connections) {
		return false;
	}

	SndMsgPtr<ChatMsgType> pMsg =
		std::make_shared<message<ChatMsgType>>(ChatMsgType::SERVER_ACCEPTED);
	sendOneClient(conn, pMsg);
	fmt::println("[{}] accepted", conn->getId());

	return true;
}

// 客户端断开连接时 调用
void ChatServer::onClientDisconnect(ConnPtr conn) {
	fmt::println("[{}] disconnected", conn->getId());
}

// 消息到达时 调用
void ChatServer::onMessage(RcvMsgPtr<ChatMsgType> pMsg) {
	ConnPtr conn = pMsg->peer;
	message<ChatMsgType>& msg = pMsg->msg;

	// fmt::println("{}", msg.data());

	// 服务器处理指定客户端的消息
	switch (msg.getId()) {
		case ChatMsgType::PING: {
			SndMsgPtr<ChatMsgType> pMsg = std::make_shared<message<ChatMsgType>>(ChatMsgType::PING);
			pMsg->writeBody(msg.data(), msg.size());
			sendOneClient(conn, pMsg);
		} break;
		case ChatMsgType::SERVER_ECHO: {
			SndMsgPtr<ChatMsgType> pMsg =
				std::make_shared<message<ChatMsgType>>(ChatMsgType::SERVER_ECHO);
			pMsg->writeBody(msg.data(), msg.size());
			sendOneClient(conn, pMsg);
		} break;
		case ChatMsgType::MESSAGE_ALL: {
			SndMsgPtr<ChatMsgType> pMsg =
				std::make_shared<message<ChatMsgType>>(ChatMsgType::MESSAGE_ALL);
			pMsg->writeBody(msg.data(), msg.size());
			sendAllClient(pMsg, conn);

		} break;
		case ChatMsgType::HEARTBEAT:
		case ChatMsgType::SERVER_ACCEPTED:
			break;
			break;
	}
}

// 服务器启动时 调用
void ChatServer::onServerStart() { checkHeartBeat(); }

// 超时心跳检测
void ChatServer::checkHeartBeat() {
	// 10s 检测一次
	m_beat_timer.expires_after(std::chrono::seconds{m_check_beat_time});

	m_beat_timer.async_wait([this](const std::error_code& ec) {
		if (ec) {
			fmt::println(stderr, "checkHeartBeat:{}, error: {}", __LINE__, ec.message());
			return;
		}

		// 遍历所有连接
		auto now = std::chrono::steady_clock::now();
		for (auto it = m_connections.begin(); it != m_connections.end();) {
			ConnPtr conn = it->second;

			using namespace std::chrono_literals;
			std::chrono::seconds duration =
				std::chrono::duration_cast<std::chrono::seconds>(now - conn->getLastActiveTime());

			// fmt::println("duration: {}", duration.count());

			// 超时 断开连接
			if (duration > std::chrono::seconds{m_beat_timeout}) {
				if (conn->isConnected()) {
					conn->disconnect();
					++it;
				} else {
					onClientDisconnect(conn);
					it = m_connections.erase(it);
				}
			} else {
				++it;
			}
		}
		// 继续检测
		if (isRunning()) checkHeartBeat();
	});
}
