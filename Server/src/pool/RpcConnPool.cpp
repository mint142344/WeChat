#include "pool/RpcConnPool.h"
#include <fmt/base.h>
#include <grpcpp/support/status.h>
#include "message.grpc.pb.h"

json RPC::getEmailVerifyCode(const std::string& email) {
	// 取出一个空闲的 Stub
	std::shared_ptr<EmailVerifyService::Stub> stub =
		RpcServiceConnPool<EmailVerifyService>::getInstance()->getConnection();
	if (!stub) {
		return {{"status", "error"}, {"message", "No available connection to email server"}};
	}

	// RAII 归还 Stub
	StubGuard<EmailVerifyService> guard(stub);

	EmailVerifyResponse response;
	ClientContext context;
	context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(5));

	EmailVerifyRequest request;
	request.set_email(email);

	Status status = guard->getEmailVerifyCode(&context, request, &response);

	if (!status.ok()) {
		if (status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
			fmt::println(stderr, "RPC::getEmailVerifyCode Timeout");
			return {{"status", "error"}, {"message", "Request timeout"}};
		}

		fmt::println(stderr, "RPC::getEmailVerifyCode {}", response.error());
		return {{"status", "error"}, {"message", response.error()}};
	}

	return {{"status", "ok"}, {"code", response.code()}};
}
