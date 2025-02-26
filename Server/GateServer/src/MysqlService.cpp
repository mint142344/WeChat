#include "MysqlService.h"
#include "DAO/ErrorCode.h"
#include "DAO/mysql/MysqlUserDao.h"
#include "DAO/user.h"

MysqlService::~MysqlService() { m_initialized = false; }

void MysqlService::init() {
	m_user_dao = std::make_unique<MysqlUserDao>();
	m_initialized = true;
}

ErrorCode MysqlService::registerUser(const std::string& username, const std::string& email,
									 const std::string& passwd) {
	if (!m_initialized) throw std::runtime_error("MysqlService not initialized");

	User user;
	user.username = username;
	user.email = email;
	user.password = passwd;

	return m_user_dao->addUser(user);
}

ErrorCode MysqlService::login(const std::string& username, const std::string& passwd) {
	if (!m_initialized) throw std::runtime_error("MysqlService not initialized");

	// 查找用户
	User user;
	ErrorCode ec = m_user_dao->queryByUsername(username, user);
	if (ec != ErrorCode::OK) return ec;

	// 验证密码
	if (user.password != PasswdHasher::passwd_hash(passwd)) {
		return ErrorCode::PASSWORD_ERROR;
	}

	return ErrorCode::OK;
}

ErrorCode MysqlService::modifyPasswd(const std::string& username, const std::string& new_passwd) {
	if (!m_initialized) throw std::runtime_error("MysqlService not initialized");

	return m_user_dao->modifyPasswd(username, new_passwd);
}

ErrorCode MysqlService::modifyAvatar(const std::string& username, const std::string& new_avatar) {
	if (!m_initialized) throw std::runtime_error("MysqlService not initialized");

	return m_user_dao->modifyAvatar(username, new_avatar);
}
