#pragma once

#include "../../Server/message.h"

// http 请求类型
enum class RequestType {
	GET_VERIFICATION_CODE, // 获取验证码
	NEW_USER,			   // 注册
	FORGET_PASSWORD,	   // 忘记密码
	LOGIN,				   // 登录
};

enum class ModuleType {
	USER,  // 用户模块(注册/找回密码)
	LOGIN, // 登录模块

};
