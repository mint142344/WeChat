#pragma once

#include "net_message.hpp"
#include "net_tsqueue.hpp"
#include "net.h"

#include <boost/asio/error.hpp>
#include <boost/uuid.hpp>
#include <fmt/base.h>

#include <chrono>
#include <memory>
#include <system_error>

/*
 *	connection 类: 负责传输消息
 *		socket 从server/client接管,由connectionn控制；
 *		server/client层 只能通过 connection 访问 socket
 *		每个connection 有自己的发送消息队列,与server/client共享接收消息队列
 *		socket(connection) 数据读/写/关闭 在io_context线程中执行
 *		消息传输:async_read async_write 保证 发送/接收完整的消息
 */
template <class T>
class connection : public std::enable_shared_from_this<connection<T>> {
public:
	enum class owner { server, client };

public:
	// who: server/client
	connection(owner who, net::io_context& ctx, tcp::socket socket,
			   tsqueue<owned_message<T>>& queue)
		: m_owner(who), m_ctx(ctx), m_socket(std::move(socket)), m_mq_recv(queue) {
		if (m_owner == owner::server) {
			// 生成 map 键值
			m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
			updateActiveTime();
		}
	}

	virtual ~connection() { m_socket.close(); }

public:
	// Note:Only Client
	void connectServer(const tcp::resolver::results_type& endpoints) {
		if (m_owner == owner::client) {
			net::async_connect(m_socket, endpoints,
							   [this](const std::error_code& ec, const tcp::endpoint& endpoint) {
								   if (ec) {
									   fmt::println(stderr, "[Connection] connect error: {}",
													ec.message());
									   m_socket.close();
									   return;
								   }

								   fmt::println("[Connection] connected to server: {}:{}",
												endpoint.address().to_string(), endpoint.port());

								   readHeader();
							   });
		}
	};

	// Note:Only Server
	void serveClient() {
		if (m_owner == owner::server && m_socket.is_open()) {
			// 监听 读事件
			readHeader();
		}
	}

	// io_context线程中关闭socket
	void disconnect() {
		if (isConnected()) net::post(m_socket.get_executor(), [this]() { m_socket.close(); });
	}

	bool isConnected() const { return m_socket.is_open(); }

	void send(SndMsgPtr<T> msg) {
		net::post(m_socket.get_executor(), [this, msg]() {
			bool is_writing = !m_mq_send.empty();
			m_mq_send.push_back(msg);
			// Note: 避免多次writeHeader
			if (!is_writing) writeHeader();
		});
	}

	// only server
	std::string getId() const { return m_uuid; }

	// only server
	std::chrono::steady_clock::time_point getLastActiveTime() const { return m_last_time; }

	// only server
	void updateActiveTime() { m_last_time = std::chrono::steady_clock::now(); }

private:
	// [ASYNC] 异步读取 message header
	void readHeader() {
		char* data = m_tmp_msg_in.header();
		size_t size = sizeof(MessageHeader<T>);

		net::async_read(m_socket, net::buffer(data, size),
						[this](const std::error_code& ec, std::size_t length) {
							if (ec) {
								// 对端关闭连接
								if (ec.value() != net::error::eof &&
									ec.value() != net::error::connection_reset) {
									fmt::println(stderr, "readHeader:{}, error: {}", __LINE__,
												 ec.message());
								}

								m_socket.close();
								return;
							}

							// fmt::println("readHeader:{}, read {} bytes", __LINE__, length);

							size_t body_size = m_tmp_msg_in.size();
							if (body_size > 0) {
								m_tmp_msg_in.resizeBody(body_size);
								// continue read body
								readBody();
							} else {
								// add msg to recv queue
								addMessageToRcvQueue();
								// continue read header
								readHeader();
							}
						});
	}

	// [ASYNC] 异步读取 message body
	void readBody() {
		char* data = m_tmp_msg_in.data();
		size_t size = m_tmp_msg_in.size();

		net::async_read(m_socket, net::buffer(data, size),
						[this](const std::error_code& ec, std::size_t length) {
							if (ec) {
								fmt::println(stderr, "readBody:{}, error: {}", __LINE__,
											 ec.message());

								m_socket.close();
								return;
							}

							// fmt::println("readBody:{} read {} bytes", __LINE__, length);

							// add msg to recv queue
							addMessageToRcvQueue();

							// continue read header
							readHeader();
						});
	}

	// [ASYNC] 异步发送message header
	void writeHeader() {
		char* data = m_mq_send.front()->header();
		size_t size = sizeof(MessageHeader<T>);

		net::async_write(m_socket, net::buffer(data, size),
						 [this](const std::error_code& ec, std::size_t length) {
							 if (ec) {
								 fmt::println(stderr, "writeHeader:{}, error: {}", __LINE__,
											  ec.message());

								 m_socket.close();
								 return;
							 }

							 //   fmt::println("writeHeader:{}, write {} bytes", __LINE__, length);

							 size_t body_size = m_mq_send.front()->size();
							 // continue write body
							 if (body_size > 0) {
								 writeBody();
							 } else {
								 m_mq_send.pop_front();

								 if (!m_mq_send.empty()) writeHeader();
							 }
						 });
	}

	// [ASYNC] 异步发送message body
	void writeBody() {
		char* data = m_mq_send.front()->data();
		size_t size = m_mq_send.front()->size();

		net::async_write(m_socket, net::buffer(data, size),
						 [this](const std::error_code& ec, std::size_t length) {
							 if (ec) {
								 fmt::println(stderr, "writeBody:{}, error: {}", __LINE__,
											  ec.message());

								 m_socket.close();
								 return;
							 }

							 //   fmt::println("writeBody:{}, write  {} bytes", __LINE__, length);

							 m_mq_send.pop_front();

							 // continue write header
							 if (!m_mq_send.empty()) writeHeader();
						 });
	}

	// 添加消息到 接收队列
	void addMessageToRcvQueue() {
		if (m_owner == owner::server) {
			auto msg = std::make_shared<owned_message<T>>(this->shared_from_this(), m_tmp_msg_in);
			m_mq_recv.push_back(msg);

			// 更新最后活跃时间
			updateActiveTime();

		} else {
			// client 只有一个connection
			auto msg = std::make_shared<owned_message<T>>(nullptr, m_tmp_msg_in);
			m_mq_recv.push_back(msg);
		}
	}

protected:
	net::io_context& m_ctx;
	tcp::socket m_socket;

	// 临时 保存接收消息
	Message<T> m_tmp_msg_in;

	// 消息发送队列
	tsqueue<Message<T>> m_mq_send;
	// 消息接收队列
	tsqueue<owned_message<T>>& m_mq_recv;

	owner m_owner = owner::server;
	// only Server 唯一uuid
	std::string m_uuid;
	// only Server
	std::chrono::steady_clock::time_point m_last_time;
};
