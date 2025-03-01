#pragma once

#include "DAO/ErrorCode.h"

#include <string>
#include <vector>

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

	// 根据用户名查询用户
	virtual ErrorCode queryByUsername(const std::string& username, User& user) = 0;

	// 根据邮箱查询用户
	virtual ErrorCode queryByEmail(const std::string& email, std::vector<User>& users) = 0;

	// 添加用户(注册)
	virtual ErrorCode addUser(const User& user) = 0;

	// 删除用户(注销)
	virtual ErrorCode deleteUser(int id) = 0;

	// 更新用户信息
	virtual ErrorCode modifyPasswd(const std::string& username, const std::string& newPasswd) = 0;
	virtual ErrorCode modifyAvatar(const std::string& username, const std::string& newAvatar) = 0;
	virtual ErrorCode modifyEmail(const std::string& username, const std::string& newEmail) = 0;
};