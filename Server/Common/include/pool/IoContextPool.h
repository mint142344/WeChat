#pragma once

#include "net.h"
#include "Singleton.hpp"

#include <memory>
#include <thread>
#include <atomic>

class IoContextPool : public Singleton<IoContextPool> {
	friend class Singleton<IoContextPool>;
	using work_guard_type = net::executor_work_guard<net::io_context::executor_type>;

public:
	IoContextPool() = default;

	IoContextPool(const IoContextPool&) = delete;
	IoContextPool(IoContextPool&&) = delete;
	IoContextPool& operator=(const IoContextPool&) = delete;
	IoContextPool& operator=(IoContextPool&&) = delete;

	~IoContextPool();

	// must & only exec once
	void start(uint32_t pool_size = 2);

	void stop();

	net::io_context& getIoContext();

private:
	std::vector<std::unique_ptr<net::io_context>> m_contexts;
	std::vector<work_guard_type> m_work_guards;
	std::vector<std::thread> m_threads;

	uint32_t m_pool_size = 0;
	uint32_t m_next_index = 0;

	std::atomic<bool> m_started{false};
};
