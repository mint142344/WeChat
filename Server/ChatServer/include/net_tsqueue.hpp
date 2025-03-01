#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>

// thread safe queue
template <class T>
class tsqueue {
	using MsgPtr = std::shared_ptr<T>;

public:
	tsqueue() = default;
	tsqueue(const tsqueue<T>&) = delete;
	tsqueue& operator=(const tsqueue&) = delete;

	MsgPtr front() {
		std::scoped_lock lock(m_mtx);
		return m_queue.front();
	}

	MsgPtr back() {
		std::scoped_lock lock(m_mtx);
		return m_queue.back();
	}

	void push_back(std::shared_ptr<T> item) {
		std::scoped_lock lock(m_mtx);
		m_queue.emplace_back(item);
		m_cv_block.notify_one();
	}

	size_t count() {
		std::scoped_lock lock(m_mtx);
		return m_queue.size();
	}

	bool empty() {
		std::scoped_lock lock(m_mtx);
		return m_queue.empty();
	}

	void clear() {
		std::scoped_lock lock(m_mtx);
		m_queue.clear();
	}

	MsgPtr pop_front() {
		std::scoped_lock lock(m_mtx);
		auto item = std::move(m_queue.front());
		m_queue.pop_front();
		return item;
	}

	void wait() {
		while (empty()) {
			std::unique_lock<std::mutex> lock(m_mtx_block);
			m_cv_block.wait(lock);
		}
	}

	void wait_for(std::chrono::milliseconds time) {
		while (empty()) {
			std::unique_lock<std::mutex> lock(m_mtx_block);
			if (m_cv_block.wait_for(lock, time) == std::cv_status::timeout) {
				break;
			}
		}
	}

private:
	std::deque<MsgPtr> m_queue;
	std::mutex m_mtx;

	std::condition_variable m_cv_block;
	std::mutex m_mtx_block;
};
