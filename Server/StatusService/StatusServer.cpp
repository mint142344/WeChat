#include "StatusServer.h"

#include <fmt/base.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <boost/uuid.hpp>
#include <boost/uuid/uuid.hpp>
#include <cstddef>
#include <mutex>
#include <shared_mutex>
#include <string>

StatusServiceImpl::StatusServiceImpl(const json& data) {
	// 读取配置文件
	for (size_t i = 0; i < data["ChatServers"].size(); ++i) {
		const auto& server = data["ChatServers"][i];
		m_chat_servers.push_back(
			{server["host"].get<std::string>(), server["port"].get<uint16_t>(), 0});

		// 打印
		fmt::println("ChatServer{}: {}:{}", i + 1, server["host"].get<std::string>(),
					 server["port"].get<uint16_t>());
	}
}

/* 客户端 发送 用户id，获取 ChatServer 信息 */
Status StatusServiceImpl::getChatServerInfo(ServerContext* context, const ChatServerRequest* req,
											ChatServerResponse* res) {
	ChatServerInfo* server_info = nullptr;

	{
		// 读锁
		std::shared_lock<std::shared_mutex> lock(m_rw_mtx);

		// 查找是否已登录(禁止一个账号多次登录)
		if (m_user_tokens.find(req->id()) != m_user_tokens.end()) {
			res->set_message("Already login");
			return {grpc::StatusCode::ALREADY_EXISTS, "Already login"};
		}

		// 查找负载最小的 ChatServer
		auto it = std::min_element(m_chat_servers.begin(), m_chat_servers.end(),
								   [](const ChatServerInfo& lhs, const ChatServerInfo& rhs) {
									   return lhs.conn_cnt < rhs.conn_cnt;
								   });

		if (it == m_chat_servers.end()) {
			res->set_message("No ChatServer");
			return {grpc::StatusCode::NOT_FOUND, "No ChatServer"};
		}

		res->set_host(it->host);
		res->set_port(std::to_string(it->port));
		server_info = &(*it);
	}

	res->set_token(generateToken());
	res->set_message("ok");

	{
		// 写锁 保存用户 信息
		std::unique_lock<std::shared_mutex> lock(m_rw_mtx);

		m_user_tokens.insert({req->id(), res->token()});
		m_user_pos.insert({req->id(), server_info});
	}

	return Status::OK;
}

Status StatusServiceImpl::loginChatServer(ServerContext* context, const LoginRequest* req,
										  LoginResponse* res) {
	{
		// 读锁 查找 user 所属 ChatServer
		std::shared_lock<std::shared_mutex> lock(m_rw_mtx);
		auto it = m_user_tokens.find(req->id());

		// 验证 token
		if (it == m_user_tokens.end()) {
			res->set_message("No token");
			return {grpc::StatusCode::NOT_FOUND, "No token"};
		}

		if (it->second != req->token()) {
			res->set_message("Token error");
			return {grpc::StatusCode::UNAUTHENTICATED, "Token error"};
		}
	}

	res->set_message("ok");

	{
		// 写锁 更新 ChatServer 连接数
		std::unique_lock<std::shared_mutex> lock(m_rw_mtx);
		++m_user_pos[req->id()]->conn_cnt;
		// fmt::println("User{} get {}:{}[{}]", req->id(), m_user_pos[req->id()]->host,
		// 			 m_user_pos[req->id()]->port, m_user_pos[req->id()]->conn_cnt);
	}

	return Status::OK;
}

std::string StatusServiceImpl::generateToken() {
	using namespace boost;
	uuids::uuid uuid = uuids::random_generator{}();

	return uuids::to_string(uuid);
}
