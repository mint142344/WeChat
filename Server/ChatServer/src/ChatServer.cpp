#include "ChatServer.h"
#include "ConfigManager.h"

#include "RpcService.h"
#include "net_message.hpp"

#include <fmt/base.h>
#include <nlohmann/json.hpp>

#include <chrono>

using json = nlohmann::json;

ChatServer::ChatServer(uint16_t port)
	: server_interface<MsgID>(port),
	  m_beat_timer(m_ctx),
	  m_beat_timeout(ConfigManager::getInstance()->beatTimeout()),
	  m_beat_duration(ConfigManager::getInstance()->checkBeatDuration()),
	  m_max_connections(ConfigManager::getInstance()->maxConnections()) {}

// 客户端连接时 调用
bool ChatServer::onClientConnect(ConnPtr conn) {
	// 超过限制，拒绝新连接
	if (m_connections.size() >= m_max_connections) {
		return false;
	}

	fmt::println("[{}] accepted", conn->getId());

	return true;
}

// 客户端断开连接时 调用
void ChatServer::onClientDisconnect(ConnPtr conn) {
	fmt::println("[{}] disconnected", conn->getId());
}

// 消息到达时 调用
void ChatServer::onMessage(RcvMsgPtr<MsgID> pMsg) {
	ConnPtr conn = pMsg->peer;
	Message<MsgID>& msg = pMsg->msg;

	fmt::println("onMessage:{}", enum_to_string(msg.getId()));

	// 服务器处理指定客户端的消息
	switch (msg.getId()) {
		case MsgID::HEARTBEAT:
			conn->updateActiveTime();

			break;

			// only server
		case MsgID::LOGIN_CHAT_SERVER:
			verifyLogin(conn, pMsg);
			break;

			// only client
		case MsgID::LOGIN_CHAT_SERVER_ACK:
			break;
	}
}

// 服务器启动时 调用
void ChatServer::onServerStart() {
	// 启动心跳检测
	checkHeartBeat();
}

// 超时心跳检测
void ChatServer::checkHeartBeat() {
	// 10s 检测一次
	m_beat_timer.expires_after(std::chrono::seconds{m_beat_duration});

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

void ChatServer::verifyLogin(ConnPtr conn, RcvMsgPtr<MsgID> pMsg) {
	char* data = pMsg->msg.data();
	size_t size = pMsg->msg.size();

	json j = json::parse(data, data + size);

	json ret = RPC::verifyToken(j["id"], j["token"]);

	SndMsgPtr<MsgID> msg = std::make_shared<Message<MsgID>>();
	msg->setId(MsgID::LOGIN_CHAT_SERVER_ACK);
	std::string res = ret.dump();
	msg->writeBody(res.data(), res.size());

	if (ret["status"] == "ok") {
		// 登录成功 发送登录成功消息
		conn->send(msg);

	} else {
		// 登录失败 发送登录失败消息
		conn->send(msg);
		conn->disconnect();
	}
}
