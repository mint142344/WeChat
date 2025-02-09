#pragma once

enum class RequestType {
	GET_VERIFICATION_CODE, // 获取验证码
	REGISTER,			   // 注册

};

enum class ModuleType {
	REGISTER, // 注册模块
	LOGIN,	  // 登录模块

};

// enum class StatusCode {
// 	SUCCESS,
// 	ERR_JSON_PARSE, // JSON解析错误
// 	ERR_NETWORK,	// 网络错误

// };