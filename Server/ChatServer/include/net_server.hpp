#pragma once

#include "net_connection.hpp"
#include "net_tsqueue.hpp"
#include "net_message.hpp"

#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <unordered_map>

// 所有操作都在io_context线程中执行确保证线程安全
template <class T>
class server_interface {
	friend class connection<T>;

	using ConnPtr = std::shared_ptr<connection<T>>;

public:
	server_interface(const server_interface&) = delete;
	server_interface(server_interface&&) = delete;
	server_interface& operator=(const server_interface&) = delete;
	server_interface& operator=(server_interface&&) = delete;

	server_interface(uint16_t port)
		: m_acceptor(m_ctx, tcp::endpoint(net::ip::tcp::v4(), port)),
		  m_signals(m_ctx, SIGINT, SIGTERM) {}

	virtual ~server_interface() { stop(); }

	bool start() noexcept {
		try {
			m_is_running = true;

			// 异步等待客户端连接
			waitForConnected();
			// 异步等待信号触发
			registerSignals();

			m_thread_ctx = std::thread([this]() { m_ctx.run(); });

			onServerStart();
		} catch (std::exception& e) {
			fmt::println(stderr, "start:{}, exception: {}", __LINE__, e.what());
			return false;
		}

		fmt::println("[Server:{}] started", __LINE__);
		return true;
	}

	void stop() {
		m_is_running = false;
		m_ctx.stop();
		if (m_thread_ctx.joinable()) m_thread_ctx.join();
		fmt::println("[Server:{}] stopped", __LINE__);
	}

	bool isRunning() const { return m_is_running; }

	// 发送 一条消息 给客户端
	void sendOneClient(ConnPtr conn, SndMsgPtr<T> msg) {
		if (conn && conn->isConnected()) {
			conn->send(msg);
		}
	}

	// 发送 消息 给所有客户端
	void sendAllClient(SndMsgPtr<T> msg, ConnPtr ignoreClient = nullptr) {
		for (auto& pair : m_connections) {
			ConnPtr& conn = pair.second;
			if (conn && conn->isConnected()) {
				if (conn != ignoreClient) {
					conn->send(msg);
				}
			}
		}
	}

	// 批量处理消息
	void handleMessage(size_t maxMessages = 4096) {
		size_t msgCount = 0;

		m_mq_recv.wait_for(std::chrono::milliseconds(100));

		while (msgCount < maxMessages && !m_mq_recv.empty()) {
			// 取出消息并处理
			onMessage(m_mq_recv.pop_front());
			msgCount++;
		}
	}

private:
	// [ASYNC] 异步等待客户端连接
	void waitForConnected() {
		m_acceptor.async_accept([this](const std::error_code& ec, net::ip::tcp::socket socket) {
			if (ec) {
				fmt::println(stderr, "waitClientConnection:{}, error: {}", __LINE__, ec.message());
				return;
			}

			fmt::println("New Connection: {}:{}", socket.remote_endpoint().address().to_string(),
						 socket.remote_endpoint().port());

			// socket由connection接管, 所有连接共享 服务器实例的消息队列引用
			// TODO: 从 io_context pool 取出一个 context
			ConnPtr conn =
				std::make_shared<connection<T>>(m_ctx, *this, std::move(socket), m_mq_recv);

			if (onClientConnect(conn)) {
				// 连接成功，添加到连接队列
				m_connections.insert({conn->getId(), conn});
				// 开始处理消息
				conn->ayncHandling();
			}

			// 继续等待下一个客户端连接
			waitForConnected();
		});
	}

	// [ASYNC] 异步信号处理
	void registerSignals() {
		m_signals.async_wait([this](const std::error_code& ec, int signal) {
			net::post(m_ctx, [this]() {
				// 断开所有连接
				m_acceptor.close();
				for (auto& pair : m_connections) {
					ConnPtr& conn = pair.second;
					conn->disconnect();
				}
				// 停止io_context
				m_is_running = false;
				m_ctx.stop();
			});
		});
	}

protected:
	// 客户端连接时 调用
	// @return: 是否允许客户端连接
	virtual bool onClientConnect(ConnPtr conn) = 0;

	// 客户端断开连接时 调用
	// 1.Socket read/write error
	// 2.Clinet explcit disconnect request
	// 3.Socket close
	virtual void onClientDisconnect(ConnPtr conn) = 0;

	// 消息到达时由 onMessage 处理
	virtual void onMessage(RcvMsgPtr<T> msg) = 0;

	// 服务器启动时 调用
	virtual void onServerStart() = 0;

protected:
	net::io_context m_ctx;
	// 服务器 唯一消息队列
	tsqueue<owned_message<T>> m_mq_recv;
	// 所有连接
	std::unordered_map<std::string, ConnPtr> m_connections;

private:
	std::thread m_thread_ctx;

	net::ip::tcp::acceptor m_acceptor;

	net::signal_set m_signals;
	std::atomic<bool> m_is_running{false};
};
