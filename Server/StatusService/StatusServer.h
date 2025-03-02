#pragma once

#include <google/protobuf/stubs/port.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>

#include <nlohmann/json.hpp>

#include <chrono>
#include <unordered_map>
#include <list>

#include "message.pb.h"
#include "message.grpc.pb.h"
#include "net.h"

using json = nlohmann::json;

using message::ChatServerRequest;
using message::ChatServerResponse;
using message::LoginRequest;
using message::LoginResponse;

using message::StatusService;

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerBuilder;

// clang-format off
inline const json DEFAULT_STATUS_CONFIG = 
{
    {"StatusServer", {
        {"ip", "127.0.0.1"},
        {"port", 6001}
    }},
    {"ChatServers", {
        {
            {"host", "localhost"},
            {"port", 6002}
        },
        {
            {"host", "localhost"},
            {"port", 6003}
        }
    }}
};
// clang-format on

// 检查状态服务器配置
inline bool checkStatusServerConfig(const json& data) {
	return data.contains("StatusServer") && data["StatusServer"].contains("ip") &&
		   data["StatusServer"].contains("port") && data.contains("ChatServers") &&
		   data["ChatServers"].is_array() && data["ChatServers"].size() > 0 &&
		   data["ChatServers"][0].contains("host") && data["ChatServers"][0].contains("port");
}

// ChatServer 信息
struct ChatServerInfo {
	std::string host;
	uint16_t port;
	uint32_t conn_cnt; // 连接数
};

struct UserInfo {
	std::string token;
	ChatServerInfo* server_info = nullptr;
	bool is_verify = false;							   // 是否 已验证
	std::chrono::steady_clock::time_point create_time; // 用户获取 ChatServer 信息的时间
};

// 状态服务器
class StatusServiceImpl final : public StatusService::Service {
public:
	StatusServiceImpl(net::io_context& ioc, const json& data);
	~StatusServiceImpl() override;

	StatusServiceImpl() = delete;
	StatusServiceImpl(const StatusServiceImpl&) = delete;
	StatusServiceImpl(StatusServiceImpl&&) = delete;
	StatusServiceImpl& operator=(const StatusServiceImpl&) = delete;
	StatusServiceImpl& operator=(StatusServiceImpl&&) = delete;

	// 获取 ChatServer 信息
	Status getChatServerInfo(ServerContext* context, const ChatServerRequest* req,
							 ChatServerResponse* res) override;

	// ChatServer 验证客户端的 登录token
	Status verifyToken(ServerContext* context, const LoginRequest* req,
					   LoginResponse* res) override;

private:
	// 验证超时时间
	static constexpr std::chrono::seconds VERIFY_TIMEOUT = std::chrono::seconds(10);

	// 生成 唯一 token
	static std::string generateToken();

	// 清理 超时 且 未验证的用户
	void cleanJunkUsers();

private:
	net::io_context& m_ioc;

	// 存储 ChatServer 信息
	std::list<ChatServerInfo> m_chat_servers;

	// key:id from mysql, value:UserInfo
	std::unordered_map<uint32_t, UserInfo> m_users;

	// 通知清理线程退出
	std::condition_variable m_cv;
	std::mutex m_mtx;

	std::thread m_clean_thread;
	std::atomic<bool> m_running{true};
};
