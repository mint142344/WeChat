#include "ConfigManger.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

using json = nlohmann::json;

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

	if (!j.is_object()) {
		throw std::runtime_error("Require json object");
	}
	if (!j.contains("gate_ip") || !j.contains("gate_port") || !j.contains("email_rpc_host") ||
		!j.contains("email_rpc_port")) {
		throw std::runtime_error("Invalid json keys");
	}

	m_gate_ip = j["gate_ip"].get<std::string>();
	m_gate_port = j["gate_port"].get<uint16_t>();
	m_email_rpc_host = j["email_rpc_host"].get<std::string>();
	m_email_rpc_port = j["email_rpc_port"].get<uint16_t>();
}

void ConfigManager::createDefaultConfig(const std::string& path) {
	std::ofstream ofs(path);

	json j;
	j["gate_ip"] = "0.0.0.0";
	j["gate_port"] = 5000;
	j["email_rpc_host"] = "127.0.0.1";
	j["email_rpc_port"] = 5001;

	ofs << j.dump(4);
	m_gate_ip = j["gate_ip"].get<std::string>();
	m_gate_port = j["gate_port"].get<uint16_t>();
	m_email_rpc_host = j["email_rpc_host"].get<std::string>();
	m_email_rpc_port = j["email_rpc_port"].get<uint16_t>();
}
