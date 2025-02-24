#include "DAO/mysql/MysqlUserDao.h"
#include "DAO/user.h"
#include "pool/MysqlConnPool.h"

#include <fmt/base.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>

ErrorCode MysqlUserDao::queryByUsername(const std::string& username, User& user) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(
			guard->prepareStatement("SELECT id,username,password,email,avatar_url,create_time FROM "
									"users WHERE BINARY username = ?"));
		pstmt->setString(1, username);

		ResPtr res(pstmt->executeQuery());
		if (res->next()) {
			user = {res->getInt(1),	   res->getString(2), res->getString(3),
					res->getString(4), res->getString(5), res->getString(6)};

			return ErrorCode::OK;
		}

		return ErrorCode::NOT_FOUND;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::queryByUsername: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::queryByEmail(const std::string& email, std::vector<User>& users) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(guard->prepareStatement("SELECT * FROM users WHERE email = ?"));
		pstmt->setString(1, email);

		ResPtr res(pstmt->executeQuery());
		while (res->next()) {
			users.emplace_back(res->getInt(1), res->getString(2), res->getString(3),
							   res->getString(4), res->getString(5), res->getString(6));
		}

		if (users.empty()) {
			return ErrorCode::NOT_FOUND;
		}

		return ErrorCode::OK;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::queryByEmail: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::addUser(const User& user) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(guard->prepareStatement(
			"INSERT INTO users (username, password, email) VALUES (?, ?, ?)"));
		pstmt->setString(1, user.username);
		pstmt->setString(2, PasswdHasher::passwd_hash(user.password));
		pstmt->setString(3, user.email);

		if (pstmt->executeUpdate() > 0) {
			return ErrorCode::OK;
		}

		return ErrorCode::DB_ERROR;
	} catch (sql::SQLException& e) {
		// 用户已存在
		if (e.getErrorCode() == 1062) {
			return ErrorCode::ALREADY_EXISTS;
		}

		fmt::println(stderr, "[{}:{}] MysqlUserDao::addUser: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::deleteUser(int id) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(guard->prepareStatement("DELETE FROM users WHERE id = ?"));
		pstmt->setInt(1, id);

		if (pstmt->executeUpdate() > 0) {
			return ErrorCode::OK;
		}

		return ErrorCode::NOT_FOUND;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::deleteUser: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::modifyPasswd(const std::string& username, const std::string& newPasswd) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(guard->prepareStatement("UPDATE users SET password = ? WHERE username = ?"));
		pstmt->setString(1, PasswdHasher::passwd_hash(newPasswd));
		pstmt->setString(2, username);

		if (pstmt->executeUpdate() > 0) {
			return ErrorCode::OK;
		}

		return ErrorCode::NOT_FOUND;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::modifyPasswd: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::modifyAvatar(const std::string& username, const std::string& newAvatar) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(
			guard->prepareStatement("UPDATE users SET avatar_url = ? WHERE username = ?"));
		pstmt->setString(1, newAvatar);
		pstmt->setString(2, username);

		if (pstmt->executeUpdate() > 0) {
			return ErrorCode::OK;
		}

		return ErrorCode::NOT_FOUND;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::modifyAvatar: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}

ErrorCode MysqlUserDao::modifyEmail(const std::string& username, const std::string& newEmail) {
	MysqlConnGuard guard(MysqlConnPool::getInstance()->getConnection());

	try {
		PstmtPtr pstmt(guard->prepareStatement("UPDATE users SET email = ? WHERE username = ?"));
		pstmt->setString(1, newEmail);
		pstmt->setString(2, username);

		if (pstmt->executeUpdate() > 0) {
			return ErrorCode::OK;
		}

		return ErrorCode::NOT_FOUND;
	} catch (sql::SQLException& e) {
		fmt::println(stderr, "[{}:{}] MysqlUserDao::modifyEmail: {}\n", e.getErrorCode(),
					 e.getSQLState(), e.what());
		return ErrorCode::DB_ERROR;
	}
}
