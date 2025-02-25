#include "ConfigManger.h"
#include <fmt/base.h>
#include <cstdio>
#include <fstream>
#include <string>

void ConfigManager::load(const std::string& path) {
	std::ifstream ifs(path);
	if (!ifs) {
		throw std::runtime_error(std::string(path) + " not found");
	}

	json data = json::parse(ifs);

	std::string error;
	if (!checkGateServerConfig(data, error)) {
		throw std::runtime_error(error + "\nPlease refer to the format below\n" +
								 DEFAULT_GATE_CONFIG.dump(4));
	}

	// init GateServer
	m_gate_ip = data["GateServer"]["ip"].get<std::string>();
	m_gate_port = data["GateServer"]["port"].get<uint16_t>();
	m_io_context_pool_size = data["GateServer"]["io_context_pool_size"].get<uint16_t>();

	// init MySQL
	m_mysql_host = data["MySQL"]["host"].get<std::string>();
	m_mysql_port = data["MySQL"]["port"].get<uint16_t>();
	m_mysql_user = data["MySQL"]["user"].get<std::string>();
	m_mysql_password = data["MySQL"]["password"].get<std::string>();
	m_mysql_db = data["MySQL"]["db"].get<std::string>();
	m_mysql_conn_pool_size = data["MySQL"]["conn_pool_size"].get<uint16_t>();

	// init Redis
	m_redis_host = data["Redis"]["host"].get<std::string>();
	m_redis_port = data["Redis"]["port"].get<uint16_t>();
	m_redis_conn_pool_size = data["Redis"]["conn_pool_size"].get<uint16_t>();

	// init RPC
	const auto& rpc = data["RPC"];
	for (const auto& service : rpc) {
		if (service.contains("EmailService")) {
			m_rpc_email_host = service["EmailService"]["host"].get<std::string>();
			m_rpc_email_port = service["EmailService"]["port"].get<uint16_t>();
			m_rpc_email_service_pool_size =
				service["EmailService"]["conn_pool_size"].get<uint16_t>();
		}
		if (service.contains("StatusService")) {
			m_rpc_status_host = service["StatusService"]["host"].get<std::string>();
			m_rpc_status_port = service["StatusService"]["port"].get<uint16_t>();
			m_rpc_status_service_pool_size =
				service["StatusService"]["conn_pool_size"].get<uint16_t>();
		}
	}
}

void ConfigManager::genDefaultConfig(const std::string& path) {
	std::ofstream ofs(path);
	if (!ofs) {
		throw std::runtime_error("Failed to create " + path);
	}

	ofs << DEFAULT_GATE_CONFIG.dump(4);
}

bool ConfigManager::checkGateServerConfig(const json& data, std::string& error) {
	if (!data.contains("GateServer") || !data.contains("MySQL") || !data.contains("RPC") ||
		!data.contains("Redis")) {
		error = "required GateServer, MySQL, RPC, Redis Object";
		return false;
	}

	// check GateServer
	const auto& gate = data["GateServer"];
	if (!gate.contains("ip") || !gate.contains("port") || !gate.contains("io_context_pool_size")) {
		error = "required GateServer.ip, GateServer.port, io_context_pool_size";
		fmt::println(stderr, "Your config:\n{}", gate.dump(4));
		return false;
	}

	// check MySQL
	const auto& mysql = data["MySQL"];
	if (!mysql.contains("host") || !mysql.contains("port") || !mysql.contains("user") ||
		!mysql.contains("password") || !mysql.contains("db") || !mysql.contains("conn_pool_size")) {
		error =
			"required MySQL.host, MySQL.port, MySQL.user, MySQL.password, MySQL.db, "
			"MySQL.conn_pool_size";
		fmt::println(stderr, "Your config:\n{}", mysql.dump(4));
		return false;
	}

	// check RPC
	const auto& redis = data["Redis"];
	if (!redis.contains("host") || !redis.contains("port") || !redis.contains("conn_pool_size")) {
		error = "required Redis.host, Redis.port, Redis.conn_pool_size";
		fmt::println(stderr, "Your config:\n{}", redis.dump(4));
		return false;
	}

	// check RPC Service
	const auto& rpc = data["RPC"];
	if (!rpc.is_array() || rpc.empty()) {
		error = "required RPC Array";
		fmt::println(stderr, "Your config:\n{}", rpc.dump(4));
		return false;
	}

	bool has_email = false;
	bool has_status = false;
	for (const auto& service : rpc) {
		if (service.contains("EmailService")) {
			has_email = true;
			if (!service["EmailService"].contains("host") ||
				!service["EmailService"].contains("port") ||
				!service["EmailService"].contains("conn_pool_size")) {
				error = "required RPC.host, RPC.port, RPC.conn_pool_size";

				fmt::println(stderr, "Your config:\n{}", service.dump(4));

				return false;
			}
		}

		if (service.contains("StatusService")) {
			has_status = true;
			if (!service["StatusService"].contains("host") ||
				!service["StatusService"].contains("port") ||
				!service["StatusService"].contains("conn_pool_size")) {
				error = "required RPC.host, RPC.port, RPC.conn_pool_size";

				fmt::println(stderr, "Your config:\n{}", service.dump(4));

				return false;
			}
		}
	}

	if (!has_email || !has_status) {
		error = "required RPC array contain: EmailService, StatusService Object";
		fmt::println(stderr, "Your config:\n{}", rpc.dump(4));
		return false;
	}

	return true;
}
