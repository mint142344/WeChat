#pragma once

#include <boost/asio.hpp>
#include <fmt/base.h>
#include <fmt/core.h>

// 枚举类转字符串
template <class Enum>
inline std::string enum_to_string(Enum e) {
	static_assert(std::is_enum_v<Enum>, "Enum type required");

	return std::to_string(static_cast<std::underlying_type_t<Enum>>(e));
}

// client/server 自定义消息类型
enum class ChatMsgType : uint32_t {
	SERVER_ACCEPTED,
	PING,
	SERVER_ECHO,
	MESSAGE_ALL,
	HEARTBEAT,
};

// 特化
template <>
inline std::string enum_to_string<ChatMsgType>(ChatMsgType e) {
	switch (e) {
		case ChatMsgType::SERVER_ACCEPTED:
			return "SERVER_ACCEPTED";
		case ChatMsgType::PING:
			return "SERVER_PING";
		case ChatMsgType::SERVER_ECHO:
			return "SERVER_ECHO";
		case ChatMsgType::MESSAGE_ALL:
			return "MESSAGE_ALL";
		default:
			return "UNKNOWN";
	}
}