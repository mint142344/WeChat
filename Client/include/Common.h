#pragma once

enum class RequestType {
	GET_VERIFICATION_CODE, // 获取验证码
	NEW_USER,			   // 注册
	FORGET_PASSWORD,	   // 忘记密码
};

enum class ModuleType {
	USER,  // 用户模块(注册/找回密码)
	LOGIN, // 登录模块

};

// enum class StatusCode {
// 	SUCCESS,
// 	ERR_JSON_PARSE, // JSON解析错误
// 	ERR_NETWORK,	// 网络错误

// };