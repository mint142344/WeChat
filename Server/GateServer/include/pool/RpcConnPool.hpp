#pragma once

#include "Singleton.hpp"
#include "message.pb.h"
#include "message.grpc.pb.h"

#include <grpcpp/channel.h>
#include <grpcpp/grpcpp.h>
#include <algorithm>

#include <memory>
#include <chrono>
#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

// 邮箱验证码服务
using message::EmailVerifyService;
using message::EmailVerifyRequest;
using message::EmailVerifyResponse;

// 状态服务
using message::StatusService;
using message::ChatServerRequest;
using message::ChatServerResponse;
using message::LoginRequest;
using message::LoginResponse;

// RPC 服务连接池 T: RPC 服务类型
template <class RpcService>
class RpcServiceConnPool final : public Singleton<RpcServiceConnPool<RpcService>> {
	friend class Singleton<RpcServiceConnPool<RpcService>>;

	using StubPtr = std::shared_ptr<typename RpcService::Stub>;

	constexpr static uint32_t MAX_POOL_SIZE = 100;

public:
	RpcServiceConnPool() = default;
	RpcServiceConnPool(const RpcServiceConnPool&) = delete;
	RpcServiceConnPool(RpcServiceConnPool&&) = delete;
	RpcServiceConnPool& operator=(const RpcServiceConnPool&) = delete;
	RpcServiceConnPool& operator=(RpcServiceConnPool&&) = delete;

	~RpcServiceConnPool() {
		std::lock_guard<std::mutex> lock(m_mtx);
		m_pool.clear();
		m_pool_size = 0;
		m_initialized = false;
	}

	void init(const std::string& host, uint16_t port, uint32_t pool_size) {
		if (pool_size <= 0) throw std::invalid_argument("pool_size must be greater than 0");

		// 防止多次初始化
		if (m_initialized) return;

		pool_size = std::min<uint32_t>(pool_size, MAX_POOL_SIZE);

		std::lock_guard<std::mutex> lock(m_mtx);

		std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + std::to_string(port),
															   grpc::InsecureChannelCredentials());

		for (int i = 0; i < pool_size; ++i) {
			std::unique_ptr<typename RpcService::Stub> stub = RpcService::NewStub(channel);

			m_pool.emplace_back(std::move(stub));
		}

		m_pool_size = pool_size;
		m_initialized = true;
	}

	bool initialized() const { return m_initialized; }

	// 返回连接池大小
	size_t size() const {
		if (!m_initialized) return 0;

		return m_pool.size();
	}

	// 返回空闲的 Stub 数量
	size_t available() {
		if (!m_initialized) return 0;

		std::lock_guard<std::mutex> lock(m_mtx);

		return m_pool.size();
	}

	// 返回空闲的 Stub
	StubPtr getConnection(std::chrono::seconds timeout = std::chrono::seconds{3}) {
		if (!initialized()) throw std::runtime_error("RpcServiceConnPool not initialized");
		std::unique_lock<std::mutex> lock(m_mtx);

		if (m_pool.empty()) {
			m_cv.wait_for(lock, timeout, [this]() { return !m_pool.empty(); });
		}

		if (m_pool.empty()) return nullptr;

		StubPtr stub = m_pool.front();
		m_pool.pop_front();

		return stub;
	}

	// 归还连接
	void releaseConnection(StubPtr stub) {
		if (!initialized()) throw std::runtime_error("RpcServiceConnPool not initialized");

		if (stub == nullptr) return;

		std::lock_guard<std::mutex> lock(m_mtx);
		m_pool.push_back(std::move(stub));
		m_cv.notify_one();
	}

private:
	std::deque<StubPtr> m_pool;
	size_t m_pool_size = 0;

	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::atomic<bool> m_initialized{false};
};

// RAII Stub 封装
template <class RpcService>
struct StubGuard {
public:
	explicit StubGuard(std::shared_ptr<typename RpcService::Stub> stub) : stub(std::move(stub)) {}

	~StubGuard() {
		if (stub) {
			RpcServiceConnPool<RpcService>::getInstance()->releaseConnection(std::move(stub));
		}
	}

	StubGuard(const StubGuard&) = delete;
	StubGuard& operator=(const StubGuard&) = delete;
	StubGuard(StubGuard&&) = default;
	StubGuard& operator=(StubGuard&&) = default;

	std::shared_ptr<typename RpcService::Stub> get() const { return stub; }

	typename RpcService::Stub* operator->() const { return stub.get(); }

	operator bool() const { return stub != nullptr; }

private:
	std::shared_ptr<typename RpcService::Stub> stub;
};
