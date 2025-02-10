#include "EmailVerifyClient.h"
#include <string>
#include "ConfigManger.h"

EmailVerifyClient::EmailVerifyClient() {
	std::string host = ConfigManager::getInstance()->m_email_rpc_host;
	std::string port = std::to_string(ConfigManager::getInstance()->m_email_rpc_port);

	m_stub = EmailVerifyService::NewStub(
		grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials()));
}

json EmailVerifyClient::getEmailVerifyCode(const std::string& email) {
	EmailVerifyResponse response;
	ClientContext context;

	EmailVerifyRequest request;
	request.set_email(email);

	Status status = m_stub->getEmailVerifyCode(&context, request, &response);

	if (!status.ok()) return {{"status", "error"}, {"message", response.error()}};

	return {{"status", "ok"}, {"code", response.code()}};
}
