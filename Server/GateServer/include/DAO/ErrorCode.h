#pragma once

enum class ErrorCode : char {
	OK = 0,
	// 数据库错误
	DB_ERROR,
	// 用户名已存在
	ALREADY_EXISTS,
	// 记录不存在
	NOT_FOUND,
	// 密码错误
	PASSWORD_ERROR,
};
