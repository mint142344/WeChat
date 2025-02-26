#include "pool/IoContextPool.h"

#include <algorithm>

IoContextPool::~IoContextPool() { stop(); }

void IoContextPool::start(uint32_t pool_size) {
	m_pool_size = pool_size;

	if (m_pool_size <= 0)
		throw std::invalid_argument("IoContextPool::start: pool_size must be greater than 0");

	m_pool_size = std::min(m_pool_size, std::thread::hardware_concurrency());

	for (int i = 0; i < m_pool_size; ++i) {
		m_contexts.emplace_back(std::make_unique<net::io_context>());
		// work_guard 绑定 io_context
		m_work_guards.emplace_back(net::make_work_guard(*m_contexts[i]));

		m_threads.emplace_back([this, i] {
			fmt::println("IoContextPool::start: thread {} started", i);
			m_contexts[i]->run();
			fmt::println("IoContextPool::start: thread {} stopped", i);
		});
	}
}

void IoContextPool::stop() {
	// 重置work_guard，io_context 在没有工作时退出
	for (auto& guard : m_work_guards) {
		guard.reset();
	}
	for (auto& ctx : m_contexts) {
		ctx->stop();
	}

	// 等待所有线程完成
	for (auto& t : m_threads) {
		t.join();
	}

	// NOTE: 清理资源
	m_threads.clear();
	m_work_guards.clear();
	m_contexts.clear();
	m_pool_size = 0;
	m_next_index = 0;
}

net::io_context& IoContextPool::getIoContext() { return *m_contexts[m_next_index++ % m_pool_size]; }