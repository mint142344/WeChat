#pragma once

#include "Common/RpcConnPool.hpp"
#include "message.pb.h"
#include "message.grpc.pb.h"

// 状态服务
using message::StatusService;
using message::LoginRequest;
using message::LoginResponse;

// RPC 服务
namespace RPC {

// ChatServer 验证 Token
json verifyToken(uint32_t id, const std::string& token);

} // namespace RPC