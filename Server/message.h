#pragma once

#include <string>
#include <cstdint>

// 客户端/服务端 遵守的消息头
template <class T>
struct MessageHeader {
	T id{};			   // 消息ID
	uint32_t size = 0; // body size
};

// 客户端/服务端 遵守的消息ID
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