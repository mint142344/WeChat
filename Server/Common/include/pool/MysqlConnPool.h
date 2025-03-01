#pragma once
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/resultset.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

#include "Singleton.hpp"

using MysqlConnPtr = std::shared_ptr<sql::Connection>;
using StmtPtr = std::unique_ptr<sql::Statement>;
using PstmtPtr = std::unique_ptr<sql::PreparedStatement>;
using ResPtr = std::unique_ptr<sql::ResultSet>;

// MySQL 连接池
class MysqlConnPool : public Singleton<MysqlConnPool> {
	friend class Singleton<MysqlConnPool>;

public:
	static constexpr uint32_t MAX_POOL_SIZE = 100;

public:
	MysqlConnPool() = default;

	MysqlConnPool(const MysqlConnPool&) = delete;
	MysqlConnPool(MysqlConnPool&&) = delete;
	MysqlConnPool& operator=(const MysqlConnPool&) = delete;
	MysqlConnPool& operator=(MysqlConnPool&&) = delete;

	~MysqlConnPool();

	// 初始化所有连接，并设置默认数据库
	void init(const std::string& host, uint16_t port, const std::string& user,
			  const std::string& password, const std::string& database, uint32_t pool_size);

	// 是否已经初始化
	bool initialized() const;

	// 检查连接池健康状态
	bool checkHealth();

	// 获取连接
	MysqlConnPtr getConnection(std::chrono::seconds timeout = std::chrono::seconds(3));

	// 归还连接
	void releaseConnection(const MysqlConnPtr& conn);

	// 连接池大小
	size_t size() const;

	// 连接池剩余连接数
	size_t available() const;

private:
	mutable std::mutex m_mtx;
	std::condition_variable m_cv;

	std::deque<MysqlConnPtr> m_pool;

	// 标记连接池是否已经初始化
	std::atomic<bool> m_initialized = false;
	size_t m_pool_size = 0;
};

// MySQL 连接 RAII 封装
struct MysqlConnGuard {
	explicit MysqlConnGuard(MysqlConnPtr conn) : m_conn(std::move(conn)) {}
	~MysqlConnGuard() {
		if (m_conn) {
			MysqlConnPool::getInstance()->releaseConnection(m_conn);
		}
	}

	MysqlConnGuard(const MysqlConnGuard&) = delete;
	MysqlConnGuard(MysqlConnGuard&&) = delete;
	MysqlConnGuard& operator=(const MysqlConnGuard&) = default;
	MysqlConnGuard& operator=(MysqlConnGuard&&) = default;

	// 获取原始连接
	MysqlConnPtr get() const { return m_conn; }

	sql::Connection* operator->() const { return m_conn.get(); }

private:
	MysqlConnPtr m_conn;
};