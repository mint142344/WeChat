#pragma once

#include "DAO/user.h"
#include "Singleton.hpp"
#include "DAO/ErrorCode.h"

#include <memory>

class MysqlUserDao;

class MysqlService : public Singleton<MysqlService> {
	friend class Singleton<MysqlService>;

public:
	MysqlService() = default;
	~MysqlService();
	MysqlService(const MysqlService&) = delete;
	MysqlService(MysqlService&&) = delete;
	MysqlService& operator=(const MysqlService&) = delete;
	MysqlService& operator=(MysqlService&&) = delete;

	void init();

	// 注册
	ErrorCode registerUser(const std::string& username, const std::string& email,
						   const std::string& passwd);

	// 登录
	ErrorCode login(const std::string& username, const std::string& passwd, User& user);

	// 修改密码
	ErrorCode modifyPasswd(const std::string& username, const std::string& new_passwd);

	// 修改头像
	ErrorCode modifyAvatar(const std::string& username, const std::string& new_avatar);

private:
	std::unique_ptr<UserDao> m_user_dao;
	bool m_initialized = false;
};