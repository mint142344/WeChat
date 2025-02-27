#include "StatusServer.h"

#include <fmt/base.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <boost/uuid.hpp>
#include <boost/uuid/uuid.hpp>

#include <chrono>
#include <mutex>
#include <thread>

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

	// 启动 清理线程
	m_clean_thread = std::thread([this] {
		fmt::println("Clean thread started");
		std::unique_lock<std::mutex> lock(m_mtx);

		while (m_running) {
			// 等待 timeout or notify
			m_cv.wait_for(lock, VERIFY_TIMEOUT, [this] { return !m_running; });

			cleanJunkUsers();
		}
	});
}

StatusServiceImpl::~StatusServiceImpl() {
	m_running = false;
	m_cv.notify_one();
	m_clean_thread.join();
}

/* 客户端 发送 用户id，获取 ChatServer 信息 */
Status StatusServiceImpl::getChatServerInfo(ServerContext* context, const ChatServerRequest* req,
											ChatServerResponse* res) {
	std::unique_lock<std::mutex> lock(m_mtx);

	// 查找是否已 成功登录
	auto it = m_users.find(req->id());

	if (it != m_users.end() && it->second.is_verify) {
		res->set_message("Already login");

		return {grpc::StatusCode::ALREADY_EXISTS, "Already login"};
	}

	// 查找负载最小的 ChatServer
	auto server_it = std::min_element(m_chat_servers.begin(), m_chat_servers.end(),
									  [](const ChatServerInfo& lhs, const ChatServerInfo& rhs) {
										  return lhs.conn_cnt < rhs.conn_cnt;
									  });

	if (server_it == m_chat_servers.end()) {
		res->set_message("No ChatServer");

		return {grpc::StatusCode::NOT_FOUND, "No ChatServer"};
	}

	// 创建或更新 用户信息
	ChatServerInfo* server_info = &(*server_it);
	std::string token = generateToken();

	if (it == m_users.end()) {
		// 第一次获取 token
		m_users[req->id()] = {token, server_info, false, std::chrono::steady_clock::now()};

	} else {
		// 未验证 未超时
		it->second.token = token;
		it->second.server_info = server_info;
		it->second.is_verify = false;
		it->second.create_time = std::chrono::steady_clock::now();
	}

	res->set_host(server_it->host);
	res->set_port(std::to_string(server_it->port));
	res->set_token(token);
	res->set_message("ok");

	return Status::OK;
}

Status StatusServiceImpl::verifyToken(ServerContext* context, const LoginRequest* req,
									  LoginResponse* res) {
	std::unique_lock<std::mutex> lock(m_mtx);

	// 查找 token
	auto it = m_users.find(req->id());

	if (it == m_users.end()) {
		res->set_message("No token");
		return {grpc::StatusCode::NOT_FOUND, "No token"};
	}

	// 验证失败 erase user
	auto cur = std::chrono::steady_clock::now();
	if (!it->second.is_verify && cur - it->second.create_time > VERIFY_TIMEOUT) {
		m_users.erase(it);
		res->set_message("Token timeout");
		return {grpc::StatusCode::DEADLINE_EXCEEDED, "Token timeout"};
	}

	if (it->second.token != req->token()) {
		m_users.erase(it);
		res->set_message("Invalid token");
		return {grpc::StatusCode::UNAUTHENTICATED, "Invalid token"};
	}

	// 验证成功 更新 ChatServer 连接数
	it->second.is_verify = true;
	++it->second.server_info->conn_cnt;

	res->set_message("ok");

	return Status::OK;
}

std::string StatusServiceImpl::generateToken() {
	using namespace boost;
	uuids::uuid uuid = uuids::random_generator{}();

	return uuids::to_string(uuid);
}

void StatusServiceImpl::cleanJunkUsers() {
	size_t cnt = 0;
	auto cur = std::chrono::steady_clock::now();

	for (auto it = m_users.begin(); it != m_users.end();) {
		if (!it->second.is_verify && cur - it->second.create_time > VERIFY_TIMEOUT) {
			it = m_users.erase(it);
			++cnt;
		} else {
			++it;
		}
	}

	// debug
	if (cnt > 0) fmt::println("Cleaned {} junk users", cnt);
}
