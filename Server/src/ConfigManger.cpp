#include "ConfigManger.h"

#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

// 网关服务器IP和端口
constexpr const char* GATE_IP = "0.0.0.0";
constexpr uint16_t GATE_PORT = 5000;

// 邮件服务器RPC IP和端口
constexpr const char* EMAIL_RPC_HOST = "127.0.0.1";
constexpr uint16_t EMAIL_RPC_PORT = 5001;

// Redis服务器IP和端口
constexpr const char* REDIS_HOST = "127.0.0.1";
constexpr uint16_t REDIS_PORT = 6379;

// ASIO io_context 池大小
constexpr uint32_t ASIO_IO_CONTEXT_POOL_SIZE = 2;
// RCP Service连接池大小
constexpr uint32_t RPC_SERVICE_CONN_POOL_SIZE = 2;
// Redis连接池大小
constexpr uint32_t REDIS_CONN_POOL_SIZE = 2;

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

	m_gate_ip = j["gate_ip"].get<std::string>();
	m_gate_port = j["gate_port"].get<uint16_t>();

	m_email_rpc_host = j["email_rpc_host"].get<std::string>();
	m_email_rpc_port = j["email_rpc_port"].get<uint16_t>();

	m_redis_host = j["redis_host"].get<std::string>();
	m_redis_port = j["redis_port"].get<uint16_t>();

	m_asio_io_context_pool_size = j["asio_io_context_pool_size"].get<uint32_t>();
	m_rpc_service_conn_pool_size = j["rpc_service_conn_pool_size"].get<uint32_t>();
	m_redis_conn_pool_size = j["redis_conn_pool_size"].get<uint32_t>();
}

void ConfigManager::createDefaultConfig(const std::string& path) {
	std::ofstream ofs(path);

	m_gate_ip = GATE_IP;
	m_gate_port = GATE_PORT;

	m_email_rpc_host = EMAIL_RPC_HOST;
	m_email_rpc_port = EMAIL_RPC_PORT;

	m_redis_host = REDIS_HOST;
	m_redis_port = REDIS_PORT;

	m_asio_io_context_pool_size = ASIO_IO_CONTEXT_POOL_SIZE;
	m_rpc_service_conn_pool_size = RPC_SERVICE_CONN_POOL_SIZE;
	m_redis_conn_pool_size = REDIS_CONN_POOL_SIZE;

	json j;
	j["gate_ip"] = m_gate_ip;
	j["gate_port"] = m_gate_port;
	j["email_rpc_host"] = m_email_rpc_host;
	j["email_rpc_port"] = m_email_rpc_port;
	j["redis_host"] = m_redis_host;
	j["redis_port"] = m_redis_port;
	j["asio_io_context_pool_size"] = m_asio_io_context_pool_size;
	j["rpc_service_conn_pool_size"] = m_rpc_service_conn_pool_size;
	j["redis_conn_pool_size"] = m_redis_conn_pool_size;

	ofs << j.dump(4);
}
