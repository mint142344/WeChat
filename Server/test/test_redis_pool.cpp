#include "pool/RedisConnPool.h"
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <fmt/format.h>

class RedisConnPoolTest : public ::testing::Test {
protected:
	void SetUp() override {
		pool = RedisConnPool::getInstance();
		// 初始化连接池，使用本地Redis服务器
		pool->init("127.0.0.1", 6379, 4);

		EXPECT_EQ(pool->available(), pool->size()) << "Initial pool size mismatch";
	}

	void TearDown() override {
		// 清理 redis-server 的测试数据
		auto conn = pool->getConnection();
		if (conn) {
			conn->del("test_key");
			conn->del("test_list");
			conn->del("test_hash");

			pool->releaseConnection(conn);
		}
		EXPECT_EQ(pool->available(), pool->size()) << "Final pool size mismatch";
	}

	RedisConnPool* pool;
};

// 测试基本的连接获取和释放
TEST_F(RedisConnPoolTest, BasicConnectionTest) {
	std::cout << pool << "\n";
	EXPECT_EQ(pool->available(), pool->size());

	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);
	EXPECT_TRUE(conn->isVaild());
	EXPECT_EQ(pool->available(), pool->size() - 1);

	pool->releaseConnection(conn);
	EXPECT_EQ(pool->available(), pool->size());
}

// 测试基本的Redis操作
TEST_F(RedisConnPoolTest, BasicOperationsTest) {
	std::cout << pool << "\n";
	EXPECT_EQ(pool->available(), pool->size());

	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);

	// 测试SET/GET
	EXPECT_TRUE(conn->set("test_key", "test_value"));
	std::string value;
	EXPECT_TRUE(conn->get("test_key", value));
	EXPECT_EQ(value, "test_value");

	// 测试EXISTS
	EXPECT_TRUE(conn->exists("test_key"));

	// 测试DEL
	EXPECT_TRUE(conn->del("test_key"));
	EXPECT_FALSE(conn->exists("test_key"));

	pool->releaseConnection(conn);

	EXPECT_EQ(pool->available(), pool->size());
}

// 测试列表操作
TEST_F(RedisConnPoolTest, ListOperationsTest) {
	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);

	// 测试LPUSH/RPOP
	EXPECT_TRUE(conn->LPush("test_list", "value1"));
	EXPECT_TRUE(conn->LPush("test_list", "value2"));

	std::string value;
	EXPECT_TRUE(conn->RPop("test_list", value));
	EXPECT_EQ(value, "value1");

	pool->releaseConnection(conn);
}

// 测试哈希表操作
TEST_F(RedisConnPoolTest, HashOperationsTest) {
	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);

	EXPECT_TRUE(conn->HSet("test_hash", "field1", "value1"));

	std::string value;
	EXPECT_TRUE(conn->HGet("test_hash", "field1", value));
	EXPECT_EQ(value, "value1");

	pool->releaseConnection(conn);
}

// 测试并发操作
TEST_F(RedisConnPoolTest, ConcurrentOperationsTest) {
	const int THREAD_COUNT = 10;
	// 每个线程操作 100 次
	const int OPS_PER_THREAD = 100;
	std::atomic<int> success_count{0};

	EXPECT_EQ(pool->available(), pool->size());

	auto worker = [&](int id) {
		for (int i = 0; i < OPS_PER_THREAD; ++i) {
			auto conn = pool->getConnection();
			if (conn) {
				std::string key = fmt::format("test_key_{}_{}", id, i);
				std::string value = fmt::format("value_{}_{}", id, i);

				if (conn->set(key, value)) {
					std::string retrieved_value;
					if (conn->get(key, retrieved_value) && retrieved_value == value) {
						success_count++;
					}
					conn->del(key.c_str());
				}
				pool->releaseConnection(conn);
			}
		}
	};

	std::vector<std::thread> threads;
	threads.reserve(THREAD_COUNT);
	for (int i = 0; i < THREAD_COUNT; ++i) {
		threads.emplace_back(worker, i);
	}

	for (auto& t : threads) {
		t.join();
	}

	EXPECT_EQ(pool->available(), pool->size());
	EXPECT_EQ(success_count, THREAD_COUNT * OPS_PER_THREAD);
}

// 测试连接超时
TEST_F(RedisConnPoolTest, ConnectionTimeoutTest) {
	std::vector<RedisConnPtr> conns;
	EXPECT_EQ(pool->available(), pool->size());

	// 耗尽连接池
	for (int i = 0; i < 5; ++i) {
		auto conn = pool->getConnection();
		if (conn) conns.push_back(conn);
	}

	EXPECT_EQ(pool->available(), 0);

	// 尝试获取更多连接，应该超时
	auto conn = pool->getConnection(std::chrono::seconds(1));
	EXPECT_EQ(conn, nullptr);

	// 释放所有连接
	for (auto& c : conns) {
		pool->releaseConnection(c);
	}
}

// 测试错误处理
TEST_F(RedisConnPoolTest, ErrorHandlingTest) {
	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);

	// 测试不存在的键
	std::string value;
	EXPECT_FALSE(conn->get("non_existent_key", value));

	// 测试错误的数据类型操作
	EXPECT_TRUE(conn->set("test_key", "test_value"));
	EXPECT_FALSE(conn->LPop("test_key", value)); // 对string类型使用LPOP应该失败

	pool->releaseConnection(conn);
}