#include "DAO/ErrorCode.h"
#include "pool/MysqlConnPool.h"
#include "DAO/user.h"
#include "DAO/mysql/MysqlUserDao.h"

#include <jdbc/cppconn/statement.h>
#include <gtest/gtest.h>
#include <memory>

class MysqlUserDaoTest : public ::testing::Test {
protected:
	void SetUp() override {
		// 初始化连接池
		MysqlConnPool* pool = MysqlConnPool::getInstance();
		pool->init("localhost", 3306, "root", "12345", "wechat", 2);

		dao = std::make_unique<MysqlUserDao>();
	}

	void TearDown() override {
		MysqlConnGuard conn(MysqlConnPool::getInstance()->getConnection());
		StmtPtr stmt(conn->createStatement());
		stmt->execute("DELETE FROM users WHERE username LIKE 'test%'");
	}

	static User createTestUser() {
		return {
			0,							 // id (auto-generated)
			"test_user",				 // username
			"123456",					 // password
			"test@example.com",			 // email
			"http://example.com/avatar", // avatar_url
			"2024-02-22 12:00:00"		 // create_time
		};
	}

	std::unique_ptr<MysqlUserDao> dao;
};

// 测试添加用户
TEST_F(MysqlUserDaoTest, AddUser) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);

	// 验证用户已添加
	auto result = dao->queryByUsername(user.username);
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result->username, user.username);
	EXPECT_EQ(result->email, user.email);
}

// 测试添加重复用户
TEST_F(MysqlUserDaoTest, AddDuplicateUser) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);
	// 添加重复用户应该失败
	EXPECT_NE(dao->addUser(user), ErrorCode::OK);
}

// 测试查询不存在的用户
TEST_F(MysqlUserDaoTest, QueryNonExistentUser) {
	auto result = dao->queryByUsername("nonexistent");
	EXPECT_FALSE(result.has_value());
}

// 测试通过邮箱查询用户
TEST_F(MysqlUserDaoTest, QueryByEmail) {
	auto user1 = createTestUser();
	auto user2 = createTestUser();
	user2.username = "test_user2";
	user2.email = user1.email; // 相同的邮箱

	EXPECT_EQ(dao->addUser(user1), ErrorCode::OK);
	EXPECT_EQ(dao->addUser(user2), ErrorCode::OK);

	auto results = dao->queryByEmail(user1.email);
	EXPECT_EQ(results.size(), 2);
}

// 测试修改密码
TEST_F(MysqlUserDaoTest, ModifyPassword) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);

	EXPECT_EQ(dao->modifyPasswd(user.username, "new_password"), ErrorCode::OK);

	auto result = dao->queryByUsername(user.username);
	ASSERT_TRUE(result.has_value());
	EXPECT_NE(result->password, user.password); // 密码应该已经被更新
}

// 测试修改头像
TEST_F(MysqlUserDaoTest, ModifyAvatar) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);

	std::string newAvatar = "http://example.com/new_avatar";
	EXPECT_EQ(dao->modifyAvatar(user.username, newAvatar), ErrorCode::OK);

	auto result = dao->queryByUsername(user.username);
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result->avatar_url, newAvatar);
}

// 测试修改邮箱
TEST_F(MysqlUserDaoTest, ModifyEmail) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);

	std::string newEmail = "new_test@example.com";
	EXPECT_EQ(dao->modifyEmail(user.username, newEmail), ErrorCode::OK);

	auto result = dao->queryByUsername(user.username);
	ASSERT_TRUE(result.has_value());
	EXPECT_EQ(result->email, newEmail);
}

// 测试删除用户
TEST_F(MysqlUserDaoTest, DeleteUser) {
	auto user = createTestUser();
	EXPECT_EQ(dao->addUser(user), ErrorCode::OK);

	auto result = dao->queryByUsername(user.username);
	ASSERT_TRUE(result.has_value());

	EXPECT_EQ(dao->deleteUser(result->id), ErrorCode::OK);

	result = dao->queryByUsername(user.username);
	EXPECT_FALSE(result.has_value());
}