#include "RpcService.h"
#include <fmt/base.h>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <string>

json RPC::errorResponse(const Status& status, const std::string& rpc_method,
						const std::string& reply_msg) {
	std::string message;

	switch (status.error_code()) {
		case grpc::StatusCode::DEADLINE_EXCEEDED:
			message = "Request timeout";

		case grpc::StatusCode::UNAVAILABLE:
			message = "Service unavailable";

		case grpc::StatusCode::ALREADY_EXISTS:
			message = "Already exists";

		default:
			break;
	}

	fmt::println(stderr, "RPC::{} {}", rpc_method, status.error_message());

	return {{"status", "error"}, {"message", message.empty() ? reply_msg : message}};
}

json RPC::getEmailVerifyCode(const std::string& email) {
	// 从邮箱验证服务连接池 取出一个空闲的 Stub
	StubGuard<EmailVerifyService> guard(
		RpcServiceConnPool<EmailVerifyService>::getInstance()->getConnection());
	if (!guard) {
		return {{"status", "error"}, {"message", "No available connection to email server"}};
	}

	// 设置超时
	ClientContext context;
	context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

	// RPC 请求
	EmailVerifyResponse response;
	EmailVerifyRequest request;
	request.set_email(email);

	Status status = guard->getEmailVerifyCode(&context, request, &response);

	if (!status.ok()) {
		return errorResponse(status, "getEmailVerifyCode", response.message());
	}

	return {{"status", "ok"}, {"message", ""}};
}

json RPC::getChatServerInfo(uint32_t id) {
	// 从状态服务连接池 取出一个空闲的 Stub
	StubGuard<StatusService> guard(
		RpcServiceConnPool<StatusService>::getInstance()->getConnection());
	if (!guard) {
		return {{"status", "error"}, {"message", "No available connection to status server"}};
	}

	// 设置 超时
	ClientContext context;
	context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

	// RPC 请求
	ChatServerResponse response;
	ChatServerRequest request;
	request.set_id(id);

	Status status = guard->getChatServerInfo(&context, request, &response);
	json data = json::object();
	data["message"] = response.message();

	if (!status.ok()) {
		return errorResponse(status, "getChatServerInfo", response.message());
	}

	return {{"status", "ok"},
			{"message", response.message()},
			{"host", response.host()},
			{"port", response.port()},
			{"token", response.token()}};
}
