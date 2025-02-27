#include "../include/RpcService.h"

json RPC::verifyToken(uint32_t id, const std::string& token) {
	// 从状态服务连接池 取出一个空闲的 Stub
	StubGuard<StatusService> guard(
		RpcServiceConnPool<StatusService>::getInstance()->getConnection());
	if (!guard) {
		return {{"status", "error"}, {"message", "No available connection to status server"}};
	}

	// 设置超时
	ClientContext context;
	context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

	// RPC 请求
	LoginResponse response;
	LoginRequest request;
	request.set_id(id);
	request.set_token(token);

	Status status = guard->verifyToken(&context, request, &response);
	if (!status.ok()) {
		return RPC::errorResponse(status, "verifyToken", response.message());
	}

	return {{"status", "ok"}, {"message", response.message()}};
}
