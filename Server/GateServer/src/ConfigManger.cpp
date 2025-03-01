#include "ConfigManger.h"
#include <fmt/base.h>
#include <fstream>
#include <string>

void ConfigManager::load(const std::string& path) {
	std::ifstream ifs(path);
	if (!ifs) {
		throw std::runtime_error(std::string(path) + " not found");
	}

	m_config = json::parse(ifs);

	std::string error;
	if (!checkGateServerConfig(m_config, error)) {
		throw std::runtime_error(error + "\nPlease refer to the format below\n" +
								 DEFAULT_GATE_CONFIG.dump(4));
	}

	fmt::println("Load {} success", path);
}

void ConfigManager::genDefaultConfig(const std::string& path) {
	std::ofstream ofs(path);
	if (!ofs) {
		throw std::runtime_error("Failed to create " + path);
	}

	ofs << DEFAULT_GATE_CONFIG.dump(4);
}

std::string ConfigManager::gateIp() const {
	return m_config["GateServer"]["ip"].get<std::string>();
}

uint16_t ConfigManager::gatePort() const { return m_config["GateServer"]["port"].get<uint16_t>(); }

uint16_t ConfigManager::ioContextPoolSize() const {
	return m_config["GateServer"]["io_context_pool_size"].get<uint16_t>();
}

std::string ConfigManager::rpcEmailHost() const {
	return m_config["RPC"][0]["EmailService"]["host"].get<std::string>();
}

uint16_t ConfigManager::rpcEmailPort() const {
	return m_config["RPC"][0]["EmailService"]["port"].get<uint16_t>();
}

uint16_t ConfigManager::rpcEmailServicePoolSize() const {
	return m_config["RPC"][0]["EmailService"]["conn_pool_size"].get<uint16_t>();
}

std::string ConfigManager::rpcStatusHost() const {
	return m_config["RPC"][1]["StatusService"]["host"].get<std::string>();
}

uint16_t ConfigManager::rpcStatusPort() const {
	return m_config["RPC"][1]["StatusService"]["port"].get<uint16_t>();
}

uint16_t ConfigManager::rpcStatusServicePoolSize() const {
	return m_config["RPC"][1]["StatusService"]["conn_pool_size"].get<uint16_t>();
}

std::string ConfigManager::redisHost() const {
	return m_config["Redis"]["host"].get<std::string>();
}

uint16_t ConfigManager::redisPort() const { return m_config["Redis"]["port"].get<uint16_t>(); }

uint16_t ConfigManager::redisConnPoolSize() const {
	return m_config["Redis"]["conn_pool_size"].get<uint16_t>();
}

std::string ConfigManager::mysqlHost() const {
	return m_config["MySQL"]["host"].get<std::string>();
}

uint16_t ConfigManager::mysqlPort() const { return m_config["MySQL"]["port"].get<uint16_t>(); }

std::string ConfigManager::mysqlUser() const {
	return m_config["MySQL"]["user"].get<std::string>();
}

std::string ConfigManager::mysqlPassword() const {
	return m_config["MySQL"]["password"].get<std::string>();
}

std::string ConfigManager::mysqlDb() const { return m_config["MySQL"]["db"].get<std::string>(); }

uint16_t ConfigManager::mysqlConnPoolSize() const {
	return m_config["MySQL"]["conn_pool_size"].get<uint16_t>();
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
