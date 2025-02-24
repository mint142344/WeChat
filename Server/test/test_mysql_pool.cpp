#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include "pool/MysqlConnPool.h"
#include <gtest/gtest.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <memory>

class MysqlConnPoolTest : public ::testing::Test {
protected:
	void SetUp() override {
		pool = MysqlConnPool::getInstance();
		pool->init("119.3.185.203", 4406, "root", "123456", "test_db", 4);
	}

	void TearDown() override {
		// Nothing to do - Singleton cleanup is automatic
	}

	MysqlConnPool* pool;
};

// 测试初始化
TEST_F(MysqlConnPoolTest, InitializationTest) {
	EXPECT_TRUE(pool->initialized());
	EXPECT_EQ(pool->size(), 4);
	EXPECT_EQ(pool->available(), 4);
}

// 测试获取连接
TEST_F(MysqlConnPoolTest, ConcurrentConnectionTest) {
	std::vector<MysqlConnPtr> connections;

	// 获取所有连接
	for (size_t i = 0; i < pool->size(); ++i) {
		auto conn = pool->getConnection();
		ASSERT_NE(conn, nullptr);
		connections.push_back(conn);
	}

	EXPECT_EQ(pool->available(), 0);

	// 尝试获取更多连接应该返回nullptr
	auto conn = pool->getConnection(std::chrono::seconds(3));
	EXPECT_EQ(conn, nullptr);

	// 释放所有连接
	for (auto& conn : connections) {
		pool->releaseConnection(conn);
	}

	EXPECT_TRUE(pool->checkHealth());
	EXPECT_EQ(pool->available(), pool->size());
}

// 测试执行SQL
TEST_F(MysqlConnPoolTest, ExecuteSQLTest) {
	auto conn = pool->getConnection();
	ASSERT_NE(conn, nullptr);

	try {
		// 创建测试表
		std::unique_ptr<sql::Statement> stmt(conn->createStatement());
		stmt->execute(
			"CREATE TABLE IF NOT EXISTS test_table ("
			"id INT PRIMARY KEY AUTO_INCREMENT,"
			"name VARCHAR(50))");

		// 插入数据
		stmt->execute("INSERT INTO test_table (name) VALUES ('test')");

		// 查询数据
		std::unique_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT * FROM test_table"));
		EXPECT_TRUE(res->next());
		EXPECT_EQ(res->getString("name"), "test");

		// 清理
		stmt->execute("DROP TABLE test_table");

	} catch (const sql::SQLException& e) {
		FAIL() << "SQL Exception: " << e.what();
	}

	pool->releaseConnection(conn);
}

// 测试超时
TEST_F(MysqlConnPoolTest, TimeoutTest) {
	std::vector<MysqlConnPtr> connections;

	connections.reserve(pool->size());
	// 耗尽连接池
	for (size_t i = 0; i < pool->size(); ++i) {
		connections.push_back(pool->getConnection());
	}

	// 尝试获取连接应该超时
	auto start = std::chrono::steady_clock::now();
	auto conn = pool->getConnection(std::chrono::seconds(1));
	auto end = std::chrono::steady_clock::now();

	EXPECT_EQ(conn, nullptr);
	EXPECT_GE(std::chrono::duration_cast<std::chrono::seconds>(end - start).count(), 1);

	// 释放连接
	for (auto& c : connections) {
		pool->releaseConnection(c);
	}
}
