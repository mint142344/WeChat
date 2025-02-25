#pragma once

#include "Singleton.hpp"

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// clang-format off
inline const json DEFAULT_GATE_CONFIG = {
        {"GateServer", {
            {"io_context_pool_size", 2},
            {"ip", "0.0.0.0"},
            {"port", 5000}
        }},
        {"MySQL", {
            {"conn_pool_size", 2},
            {"host", "localhost"},
            {"port", 3306},
            {"user", "root"},
            {"password", "123456"},
            {"db", "wechat"}
        }},
        {"RPC", {
            {
                {"EmailService", {
                    {"conn_pool_size", 2},
                    {"host", "127.0.0.1"},
                    {"port", 5001}
                }}
            },
            {
                {"StatusService", {
                    {"conn_pool_size", 2},
                    {"host", "localhost"},
                    {"port", 6001}
                }}
            }
        }},
        {"Redis", {
            {"conn_pool_size", 2},
            {"host", "127.0.0.1"},
            {"port", 6379}
        }}
    };
// clang-format off

class ConfigManager : public Singleton<ConfigManager> {
	friend class Singleton<ConfigManager>;

public:
	// 配置文件路径
	static constexpr const char* CONFIG_PATH = "gate_server_config.json";

	// Mysql 连接池最大连接数
	static constexpr const uint32_t MYSQL_CONN_POOL_SIZE = 1024;
	// Redis 连接池最大连接数
	static constexpr const uint32_t REDIS_CONN_POOL_SIZE = 1024;
	// RPC 连接池最大连接数
	static constexpr const uint32_t RPC_CONN_POOL_SIZE = 1024;

public:
	ConfigManager() = default;
	~ConfigManager() = default;
	ConfigManager(const ConfigManager&) = delete;
	ConfigManager(ConfigManager&&) = delete;
	ConfigManager& operator=(const ConfigManager&) = delete;
	ConfigManager& operator=(ConfigManager&&) = delete;

	// 加载服务器配置文件
	void load(const std::string& path = CONFIG_PATH);

	// 生成默认配置文件
	static void genDefaultConfig(const std::string& path = CONFIG_PATH);

private:
	// 检查配置文件格式
	static bool checkGateServerConfig(const json& data, std::string& error);

public:
	// GateServer
	std::string m_gate_ip;
	uint16_t m_gate_port{};
	uint16_t m_io_context_pool_size{};

	// RPC 邮箱服务
	std::string m_rpc_email_host;
	uint16_t m_rpc_email_port{};
	uint16_t m_rpc_email_service_pool_size{};

	// RPC 状态服务
	std::string m_rpc_status_host;
	uint16_t m_rpc_status_port{};
	uint16_t m_rpc_status_service_pool_size{};

	// Redis
	std::string m_redis_host;
	uint16_t m_redis_port{};
	uint16_t m_redis_conn_pool_size{};

	// Mysql
	std::string m_mysql_host;
	uint16_t m_mysql_port{};
	std::string m_mysql_user;
	std::string m_mysql_password;
	std::string m_mysql_db;
	uint16_t m_mysql_conn_pool_size{};
};