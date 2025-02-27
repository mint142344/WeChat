#pragma once

#include "Common/RpcConnPool.hpp"
#include "message.pb.h"
#include "message.grpc.pb.h"

// 邮箱验证码服务
using message::EmailVerifyService;
using message::EmailVerifyRequest;
using message::EmailVerifyResponse;

// 状态服务
using message::StatusService;
using message::ChatServerRequest;
using message::ChatServerResponse;

// RPC 服务
namespace RPC {

// 获取邮箱验证码
json getEmailVerifyCode(const std::string& email);

// 获取 ChatServer 信息
json getChatServerInfo(uint32_t id);

} // namespace RPC