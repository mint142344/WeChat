#pragma once

#include "Singleton.hpp"
#include "message.pb.h"
#include "message.grpc.pb.h"

#include <grpcpp/channel.h>
#include <grpcpp/grpcpp.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

using json = nlohmann::json;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::EmailVerifyRequest;
using message::EmailVerifyResponse;
using message::EmailVerifyService;

// RPC 服务连接池 T: RPC 服务类型
template <class RpcService>
class RpcServiceConnPool final : public Singleton<RpcServiceConnPool<RpcService>> {
	friend class Singleton<RpcServiceConnPool<RpcService>>;
	using StubPtr = std::unique_ptr<typename RpcService::Stub>;

	struct Connection {
		StubPtr stub;
		bool is_busy;
	};

public:
	~RpcServiceConnPool() {}

	void init(const std::string& host, uint16_t port, uint32_t pool_size) {
		std::lock_guard<std::mutex> lock(m_mtx);
		m_pool_size = pool_size;

		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + std::to_string(port),
															   grpc::InsecureChannelCredentials());

		for (int i = 0; i < pool_size; ++i) {
			m_stubs.push_back({RpcService::NewStub(channel), false});
		}
	}

	// 返回空闲的 Stub
	StubPtr get(std::chrono::seconds timeout = std::chrono::seconds{3}) {
		std::unique_lock<std::mutex> lock(m_mtx);

		bool ok = m_cv.wait_for(lock, timeout, [this]() {
			return std::any_of(m_stubs.begin(), m_stubs.end(),
							   [](const Connection& conn) { return !conn.is_busy; });
		});

		if (!ok) return nullptr;

		for (auto& conn : m_stubs) {
			if (!conn.is_busy) {
				conn.is_busy = true;
				return std::move(conn.stub);
			}
		}
		return nullptr;
	}

	// 归还连接
	void put(StubPtr stub) {
		std::lock_guard<std::mutex> lock(m_mtx);
		auto it = std::find_if(m_stubs.begin(), m_stubs.end(),
							   [](Connection& conn) { return conn.is_busy; });

		if (it != m_stubs.end()) {
			it->stub = std::move(stub);
			it->is_busy = false;
			m_cv.notify_one();
		}
	}

private:
	uint32_t m_pool_size;
	std::vector<Connection> m_stubs;

	std::mutex m_mtx;
	std::condition_variable m_cv;
};

namespace RPC {

json getEmailVerifyCode(const std::string& email);

} // namespace RPC