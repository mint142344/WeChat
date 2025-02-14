#include "ConfigManger.h"

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

// 网关服务器 IP, 端口, ASIO io_context 池大小
constexpr const char* GATE_IP = "0.0.0.0";
constexpr uint16_t GATE_PORT = 5000;
constexpr uint32_t ASIO_IO_CONTEXT_POOL_SIZE = 2;

// RPC服务器 IP,端口, 连接池大小
constexpr const char* EMAIL_RPC_HOST = "127.0.0.1";
constexpr uint16_t EMAIL_RPC_PORT = 5001;
constexpr uint32_t RPC_SERVICE_CONN_POOL_SIZE = 2;

// Redis服务器 IP, 端口, 连接池大小
constexpr const char* REDIS_HOST = "127.0.0.1";
constexpr uint16_t REDIS_PORT = 6379;
constexpr uint32_t REDIS_CONN_POOL_SIZE = 2;

// MySQL服务器 IP, 端口, 用户名, 密码, 连接池大小
constexpr const char* MYSQL_HOST = "127.0.0.1";
constexpr uint16_t MYSQL_PORT = 3306;
constexpr const char* MYSQL_USER = "root";
constexpr const char* MYSQL_PASSWORD = "12345";
constexpr uint32_t MYSQL_CONN_POOL_SIZE = 2;

void ConfigManager::load(const std::string& path) {
	std::ifstream ifs(path);

	// 不存在  创建写入默认配置
	if (!ifs) {
		createDefaultConfig(path);
		return;
	}

	std::string data;
	data.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	json j;
	if (json::accept(data)) {
		j = json::parse(data);
	} else {
		throw std::runtime_error("Invalid json config file");
	}

	// Gate Server
	m_gate_ip = j["GateServer"]["gate_ip"].get<std::string>();
	m_gate_port = j["GateServer"]["gate_port"].get<uint16_t>();
	m_asio_io_context_pool_size = j["GateServer"]["asio_io_context_pool_size"].get<uint32_t>();

	// RPC Server
	m_rpc_host = j["RPC"]["rpc_host"].get<std::string>();
	m_rpc_port = j["RPC"]["rpc_port"].get<uint16_t>();
	m_rpc_service_conn_pool_size = j["RPC"]["rpc_conn_pool_size"].get<uint32_t>();

	// Redis Server
	m_redis_host = j["Redis"]["redis_host"].get<std::string>();
	m_redis_port = j["Redis"]["redis_port"].get<uint16_t>();
	m_redis_conn_pool_size = j["Redis"]["redis_conn_pool_size"].get<uint32_t>();

	// MySQL Server
	m_mysql_host = j["MySQL"]["mysql_host"].get<std::string>();
	m_mysql_port = j["MySQL"]["mysql_port"].get<uint16_t>();
	m_mysql_user = j["MySQL"]["mysql_user"].get<std::string>();
	m_mysql_password = j["MySQL"]["mysql_password"].get<std::string>();
	m_mysql_conn_pool_size = j["MySQL"]["mysql_conn_pool_size"].get<uint32_t>();
}

void ConfigManager::createDefaultConfig(const std::string& path) {
	std::ofstream ofs(path);

	// Gate Server
	m_gate_ip = GATE_IP;
	m_gate_port = GATE_PORT;
	m_asio_io_context_pool_size = ASIO_IO_CONTEXT_POOL_SIZE;

	// RPC Server
	m_rpc_host = EMAIL_RPC_HOST;
	m_rpc_port = EMAIL_RPC_PORT;
	m_rpc_service_conn_pool_size = RPC_SERVICE_CONN_POOL_SIZE;

	// Redis Server
	m_redis_host = REDIS_HOST;
	m_redis_port = REDIS_PORT;
	m_redis_conn_pool_size = REDIS_CONN_POOL_SIZE;

	// MySQL Server
	m_mysql_host = MYSQL_HOST;
	m_mysql_port = MYSQL_PORT;
	m_mysql_user = MYSQL_USER;
	m_mysql_password = MYSQL_PASSWORD;
	m_mysql_conn_pool_size = MYSQL_CONN_POOL_SIZE;

	json j;
	j["GateServer"]["gate_ip"] = m_gate_ip;
	j["GateServer"]["gate_port"] = m_gate_port;
	j["GateServer"]["asio_io_context_pool_size"] = m_asio_io_context_pool_size;

	j["RPC"]["rpc_host"] = m_rpc_host;
	j["RPC"]["rpc_port"] = m_rpc_port;
	j["RPC"]["rpc_conn_pool_size"] = m_rpc_service_conn_pool_size;

	j["Redis"]["redis_host"] = m_redis_host;
	j["Redis"]["redis_port"] = m_redis_port;
	j["Redis"]["redis_conn_pool_size"] = m_redis_conn_pool_size;

	j["MySQL"]["mysql_host"] = m_mysql_host;
	j["MySQL"]["mysql_port"] = m_mysql_port;
	j["MySQL"]["mysql_user"] = m_mysql_user;
	j["MySQL"]["mysql_password"] = m_mysql_password;
	j["MySQL"]["mysql_conn_pool_size"] = m_mysql_conn_pool_size;

	ofs << j.dump(4);
}
