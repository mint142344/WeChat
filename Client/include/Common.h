#pragma once

#include <cstdint>
#include <vector>
#include <QString>

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

// enum class StatusCode {
// 	SUCCESS,
// 	ERR_JSON_PARSE, // JSON解析错误
// 	ERR_NETWORK,	// 网络错误

// };

// 消息头
template <class T>
struct message_header {
	T id{};			   // 用户自定义的枚举类
	uint32_t size = 0; // body size
};

// 存储要发送的消息; T:枚举类
// template <class T>
// struct message {
// private:
// 	message_header<T> m_header{};
// 	std::vector<uint8_t> m_body;

// public:
// 	void setId(T id) { m_header.id = id; }
// 	T getId() const { return m_header.id; }

// 	char* header() { return (char*)&m_header; }
// 	char* data() { return (char*)m_body.data(); }

// 	// body size
// 	size_t size() const { return m_header.size; }

// 	void setBody(const void* data, size_t size) {
// 		m_body.clear();
// 		m_body.resize(size);
// 		memcpy(m_body.data(), data, size);
// 		m_header.size = size;
// 	}
// };

enum class MsgID : uint32_t {
	// 登录聊天服务器
	LOGIN_CHAT_SERVER,
	LOGIN_CHAT_SERVER_ACK,

	// 心跳
	HEARTBEAT,
};

// 枚举类转字符串
template <class Enum>
inline std::string enum_to_string(Enum e) {
	static_assert(std::is_enum_v<Enum>, "Enum type required");

	return std::to_string(static_cast<std::underlying_type_t<Enum>>(e));
}

// 特化
template <>
inline std::string enum_to_string<MsgID>(MsgID e) {
	switch (e) {
		case MsgID::LOGIN_CHAT_SERVER:
			return "LOGIN_CHAT_SERVER";

		case MsgID::LOGIN_CHAT_SERVER_ACK:
			return "LOGIN_CHAT_SERVER_ACK";

		case MsgID::HEARTBEAT:
			return "HEARTBEAT";
		default:
			return "UNKNOWN";
	}
}