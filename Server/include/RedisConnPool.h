#pragma once

#include "Singleton.hpp"

#include <hiredis/hiredis.h>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>

// Redis 连接
class RedisConnection {
public:
	RedisConnection(redisContext* context = nullptr);
	~RedisConnection();

public:
	bool auth(const std::string& password);

	bool set(const std::string& key, const std::string& value);
	bool get(const std::string& key, std::string& value);
	bool del(const char* key);

	bool LPush(const std::string& key, const std::string& value);
	bool LPop(const std::string& key, std::string& value);
	bool RPush(const std::string& key, const std::string& value);
	bool RPop(const std::string& key, std::string& value);

	bool HSet(const std::string& key, const std::string& field, const std::string& value);
	bool HGet(const std::string& key, const std::string& field, std::string& value);

	bool exists(const std::string& key);

	bool isVaild() const;

	bool expire(const std::string& key, int seconds);

private:
	redisContext* m_context;
};

using RedisConnPtr = std::shared_ptr<RedisConnection>;
// Redis 连接池
class RedisConnPool : public Singleton<RedisConnPool> {
	friend class Singleton<RedisConnPool>;

public:
	~RedisConnPool();

	void init(const std::string& ip, uint16_t port, uint32_t pool_size);

	// 获取连接
	RedisConnPtr getConnection(std::chrono::seconds timeout = std::chrono::seconds(3));

	// 归还连接
	void releaseConnection(RedisConnPtr conn);

	size_t available();

	size_t size() const;

private:
	std::deque<RedisConnPtr> m_connections;

	uint32_t m_pool_size;

	std::mutex m_mtx;
	std::condition_variable m_cv;
};