#pragma once

#include <string>
#include <vector>
#include <optional>

struct User {
	User() = default;
	User(int id, std::string username, std::string password, std::string email, std::string avatar,
		 std::string create_time)
		: id(id),
		  username(std::move(username)),
		  password(std::move(password)),
		  email(std::move(email)),
		  avatar_url(std::move(avatar)),
		  create_time(std::move(create_time)) {}

	int id{};
	std::string username;
	std::string password;
	std::string email;
	std::string avatar_url;
	std::string create_time;
};

class UserDao {
public:
	UserDao() = default;

	UserDao(const UserDao&) = delete;
	UserDao(UserDao&&) = delete;
	UserDao& operator=(const UserDao&) = delete;
	UserDao& operator=(UserDao&&) = delete;

	virtual ~UserDao() = default;

protected:
	// 根据用户名查询用户
	virtual std::optional<User> queryByUsername(const std::string& username) = 0;

	// 根据邮箱查询用户
	virtual std::vector<User> queryByEmail(const std::string& email) = 0;

	// 添加用户(注册)
	virtual bool addUser(const User& user) = 0;

	// 删除用户(注销)
	virtual bool deleteUser(int id) = 0;

	// 更新用户信息
	virtual bool modifyPasswd(const std::string& username, const std::string& newPasswd) = 0;
	virtual bool modifyAvatar(const std::string& username, const std::string& newAvatar) = 0;
	virtual bool modifyEmail(const std::string& username, const std::string& newEmail) = 0;
};