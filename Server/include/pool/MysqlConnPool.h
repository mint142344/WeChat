#pragma once
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>

#include "Singleton.hpp"

using MysqlConnPtr = std::shared_ptr<sql::Connection>;

class MysqlConnPool : public Singleton<MysqlConnPool> {
	friend class Singleton<MysqlConnPool>;

public:
	~MysqlConnPool();

	void init(const std::string& host, uint16_t port, const std::string& user,
			  const std::string& password, const std::string& database, uint32_t pool_size);

	// 是否已经初始化
	bool initialized() const;

	// 检查连接池健康状态
	bool checkHealth();

	// 获取连接
	MysqlConnPtr getConnection(std::chrono::seconds timeout = std::chrono::seconds(3));

	// 归还连接
	void releaseConnection(MysqlConnPtr conn);

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
