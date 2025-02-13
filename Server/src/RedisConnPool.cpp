#include "RedisConnPool.h"

#include <hiredis/hiredis.h>
#include <cassert>
#include <fmt/core.h>
#include <memory>
#include <mutex>
#include <string>

RedisConnection::RedisConnection(redisContext* context) : m_context{context} {}

RedisConnection::~RedisConnection() {
	if (m_context) {
		redisFree(m_context);
		m_context = nullptr;
	}
}

bool RedisConnection::auth(const std::string& password) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "AUTH %s", password.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0);

	freeReplyObject(reply);
	return ok;
}

bool RedisConnection::set(const std::string& key, const std::string& value) {
	assert(m_context);

	redisReply* reply =
		(redisReply*)redisCommand(m_context, "SET %s %s", key.c_str(), value.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_STATUS && strcasecmp(reply->str, "OK") == 0);

	freeReplyObject(reply);
	return ok;
}

bool RedisConnection::get(const std::string& key, std::string& value) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "GET %s", key.c_str());
	if (!reply) return false;

	bool ok = false;
	if (reply->type == REDIS_REPLY_STRING) {
		value = reply->str;
		ok = true;
	}

	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::del(const char* key) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "DEL %s", key);
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::LPush(const std::string& key, const std::string& value) {
	assert(m_context);

	redisReply* reply =
		(redisReply*)redisCommand(m_context, "LPUSH %s %s", key.c_str(), value.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer >= 0);
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::LPop(const std::string& key, std::string& value) {
	assert(m_context);
	redisReply* reply = (redisReply*)redisCommand(m_context, "LPOP %s", key.c_str());
	if (!reply) return false;

	bool ok = false;
	if (reply->type == REDIS_REPLY_STRING) {
		value = reply->str;
		ok = true;
	}
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::RPush(const std::string& key, const std::string& value) {
	assert(m_context);

	redisReply* reply =
		(redisReply*)redisCommand(m_context, "RPUSH %s %s", key.c_str(), value.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer >= 0);
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::RPop(const std::string& key, std::string& value) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "RPOP %s", key.c_str());
	if (!reply) return false;

	bool ok = false;
	if (reply->type == REDIS_REPLY_STRING) {
		value = reply->str;
		ok = true;
	}
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::HSet(const std::string& key, const std::string& field,
						   const std::string& value) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "HSET %s %s %s", key.c_str(),
												  field.c_str(), value.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::HGet(const std::string& key, const std::string& field, std::string& value) {
	assert(m_context);

	redisReply* reply =
		(redisReply*)redisCommand(m_context, "HGET %s %s", key.c_str(), field.c_str());

	if (!reply) return false;

	bool ok = false;
	if (reply->type == REDIS_REPLY_STRING) {
		value = reply->str;
		ok = true;
	}
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::exists(const std::string& key) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "EXISTS %s", key.c_str());
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);
	freeReplyObject(reply);

	return ok;
}

bool RedisConnection::isVaild() const { return m_context != nullptr && m_context->err == 0; }

bool RedisConnection::expire(const std::string& key, int seconds) {
	assert(m_context);

	redisReply* reply = (redisReply*)redisCommand(m_context, "EXPIRE %s %d", key.c_str(), seconds);
	if (!reply) return false;

	bool ok = (reply->type == REDIS_REPLY_INTEGER && reply->integer == 1);

	freeReplyObject(reply);
	return ok;
}

//////////////////////////////////////////////////////////////////////////
RedisConnPool::~RedisConnPool() {
	std::lock_guard<std::mutex> lock(m_mtx);
	m_connections.clear();
	m_pool_size = 0;
}

void RedisConnPool::init(const std::string& ip, uint16_t port, uint32_t pool_size) {
	if (pool_size <= 0) throw std::invalid_argument("pool_size must be greater than 0");

	if (!m_connections.empty()) return;

	m_pool_size = pool_size;

	std::lock_guard<std::mutex> lock(m_mtx);

	for (int i = 0; i < m_pool_size; ++i) {
		redisContext* context = redisConnect(ip.c_str(), port);
		if (context == nullptr || context->err) {
			if (context) {
				fmt::print("Error: {}\n", context->errstr);
				redisFree(context);
			} else {
				fmt::print("Can't allocate redis context\n");
			}
			throw std::runtime_error("Can't connect to redis server");
		}

		m_connections.emplace_back(std::make_shared<RedisConnection>(context));
	}
}

RedisConnPtr RedisConnPool::getConnection(std::chrono::seconds timeout) {
	std::unique_lock<std::mutex> lock(m_mtx);

	if (m_connections.empty()) {
		m_cv.wait_for(lock, timeout, [this] { return !m_connections.empty(); });
	}

	if (m_connections.empty())
		return nullptr;
	else {
		RedisConnPtr conn = m_connections.front();
		m_connections.pop_front();
		return conn;
	}
}

void RedisConnPool::releaseConnection(RedisConnPtr conn) {
	if (!conn) return;

	std::lock_guard<std::mutex> lock(m_mtx);

	m_connections.emplace_back(conn);
	m_cv.notify_one();
}

size_t RedisConnPool::available() {
	std::lock_guard<std::mutex> lock(m_mtx);
	return m_connections.size();
}

size_t RedisConnPool::size() const { return m_pool_size; }
