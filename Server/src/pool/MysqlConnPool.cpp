#include "pool/MysqlConnPool.h"

#include <fmt/base.h>
#include <jdbc/cppconn/connection.h>
#include <jdbc/mysql_driver.h>
#include <fmt/format.h>

#include <algorithm>
#include <mutex>
#include <string>

MysqlConnPool::~MysqlConnPool() {
	std::lock_guard<std::mutex> lock(m_mtx);
	m_pool.clear();
	m_pool_size = 0;
	m_initialized = false;
}

void MysqlConnPool::init(const std::string& host, uint16_t port, const std::string& user,
						 const std::string& password, const std::string& database,
						 uint32_t pool_size) {
	if (pool_size <= 0) throw std::invalid_argument("pool_size must be greater than 0");

	std::lock_guard<std::mutex> lock(m_mtx);

	// 防止重复初始化
	if (m_initialized) return;

	try {
		sql::mysql::MySQL_Driver* driver = sql::mysql::get_driver_instance();

		for (int i = 0; i < pool_size; ++i) {
			sql::Connection* conn =
				driver->connect("tcp://" + host + ":" + std::to_string(port), user, password);
			if (conn == nullptr) {
				throw std::runtime_error("Failed to connect to database");
			}
			conn->setSchema(database);

			m_pool.emplace_back(conn);
		}
	} catch (const sql::SQLException& e) {
		throw std::runtime_error(fmt::format("MySQL conn pool: {}", e.what()));
	}

	m_pool_size = pool_size;
	m_initialized = true;
}

bool MysqlConnPool::initialized() const { return m_initialized; }

bool MysqlConnPool::checkHealth() {
	if (!initialized()) throw std::runtime_error("MysqlConnPool not initialized");

	std::lock_guard<std::mutex> lock(m_mtx);

	return std::all_of(m_pool.begin(), m_pool.end(), [](const MysqlConnPtr& conn) {
		return conn->isValid() && !conn->isClosed();
	});
}

MysqlConnPtr MysqlConnPool::getConnection(std::chrono::seconds timeout) {
	if (!initialized()) throw std::runtime_error("MysqlConnPool not initialized");

	std::unique_lock<std::mutex> lock(m_mtx);
	if (m_pool.empty()) {
		m_cv.wait_for(lock, timeout, [this]() { return !m_pool.empty(); });
	}

	if (m_pool.empty()) {
		return nullptr;
	}

	MysqlConnPtr conn = m_pool.front();
	m_pool.pop_front();

	// 检查连接是否有效
	try {
		if (!conn->isValid() || conn->isClosed()) {
			conn->reconnect();
		}

	} catch (const sql::SQLException& e) {
		fmt::println("{}:MySQL Error: {}", __PRETTY_FUNCTION__, e.what());
		return nullptr;
	}

	return conn;
}

void MysqlConnPool::releaseConnection(const MysqlConnPtr& conn) {
	if (!initialized()) throw std::runtime_error("MysqlConnPool not initialized");

	std::lock_guard<std::mutex> lock(m_mtx);

	m_pool.push_back(conn);
	m_cv.notify_one();
}

size_t MysqlConnPool::size() const {
	if (!initialized()) return 0;

	return m_pool_size;
}

size_t MysqlConnPool::available() const {
	if (!initialized()) return 0;

	std::lock_guard<std::mutex> lock(m_mtx);

	return m_pool.size();
}
