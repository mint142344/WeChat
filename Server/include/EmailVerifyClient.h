#pragma once

#include "Singleton.hpp"
#include "message.pb.h"
#include "message.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>

using json = nlohmann::json;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::EmailVerifyRequest;
using message::EmailVerifyResponse;
using message::EmailVerifyService;

class EmailVerifyClient : public Singleton<EmailVerifyClient> {
	friend class Singleton<EmailVerifyClient>;

public:
	EmailVerifyClient();

	json getEmailVerifyCode(const std::string& email);

private:
	std::unique_ptr<EmailVerifyService::Stub> m_stub;
};
