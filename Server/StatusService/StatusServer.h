#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <list>
#include <shared_mutex>

#include <google/protobuf/stubs/port.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>

#include <nlohmann/json.hpp>

#include "message.pb.h"
#include "message.grpc.pb.h"

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

// 状态服务器
class StatusServiceImpl final : public StatusService::Service {
public:
	StatusServiceImpl() = delete;
	StatusServiceImpl(const json& data);

	// 获取 ChatServer 信息
	Status getChatServerInfo(ServerContext* context, const ChatServerRequest* req,
							 ChatServerResponse* res) override;

	// 登录 ChatServer
	Status loginChatServer(ServerContext* context, const LoginRequest* req,
						   LoginResponse* res) override;

private:
	// 生成 唯一 token
	static std::string generateToken();

private:
	// 存储 ChatServer 信息
	std::list<ChatServerInfo> m_chat_servers;

	// key:id, value:ChatServerInfo
	std::unordered_map<uint32_t, ChatServerInfo*> m_user_pos;

	// key:id, value:token
	std::unordered_map<uint32_t, std::string> m_user_tokens;

	std::shared_mutex m_rw_mtx;
};
