#pragma once

#include "pool/RpcConnPool.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// RPC 服务
namespace RPC {

// 统一错误处理
json errorResponse(const Status& status, const std::string& rpc_method,
				   const std::string& reply_msg);

// 获取邮箱验证码
json getEmailVerifyCode(const std::string& email);

// 获取 ChatServer 信息
json getChatServerInfo(uint32_t id);

} // namespace RPC