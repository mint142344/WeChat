#pragma once

#include "pool/RpcConnPool.hpp"
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

// 状态服务
using message::StatusService;
using message::LoginRequest;
using message::LoginResponse;
using message::LogoutRequest;
using message::LogoutResponse;

// RPC 服务 客户端
namespace RPC {

// 获取邮箱验证码
json getEmailVerifyCode(const std::string& email);

// 获取 ChatServer 信息
json getChatServerInfo(uint32_t id);

// ChatServer 验证 Token
json verifyToken(uint32_t id, const std::string& token);

// ChatServer 通知 StatuServer 用户登出
json userLogout(uint32_t id, const std::string& token);

} // namespace RPC