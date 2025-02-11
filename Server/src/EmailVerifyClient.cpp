#include "EmailVerifyClient.h"
#include "message.grpc.pb.h"

json RPC::getEmailVerifyCode(const std::string& email) {
	// 取出一个空闲的 Stub
	std::unique_ptr<EmailVerifyService::Stub> stub =
		RpcServiceConnPool<EmailVerifyService>::getInstance()->get();
	if (!stub) {
		return {{"status", "error"}, {"message", "No available connection to email server"}};
	}

	// RAII 归还 Stub
	struct StubGuard {
		std::unique_ptr<EmailVerifyService::Stub> stub;
		~StubGuard() {
			RpcServiceConnPool<EmailVerifyService>::getInstance()->put(std::move(stub));
		}
	} guard{std::move(stub)};

	EmailVerifyResponse response;
	ClientContext context;

	EmailVerifyRequest request;
	request.set_email(email);

	Status status = guard.stub->getEmailVerifyCode(&context, request, &response);

	if (!status.ok()) {
		return {{"status", "error"}, {"message", response.error()}};
	}

	return {{"status", "ok"}, {"code", response.code()}};
}
