#pragma once

#include "net_message.hpp"
#include "net_tsqueue.hpp"
#include "net.h"

#include <boost/asio/error.hpp>
#include <boost/uuid.hpp>
#include <fmt/base.h>

#include <chrono>
#include <memory>
#include <optional>
#include <system_error>
#include <map>
#include <any>

template <class T>
class server_interface;

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
	connection(const connection&) = delete;
	connection(connection&&) = default;
	connection& operator=(const connection&) = delete;
	connection& operator=(connection&&) = default;

	// who: server/client
	connection(net::io_context& ctx, server_interface<T>& server, tcp::socket socket,
			   tsqueue<owned_message<T>>& queue)
		: m_ctx(ctx), m_server(server), m_socket(std::move(socket)), m_mq_recv(queue) {
		// 生成 TCP 连接标识
		m_uuid = boost::uuids::to_string(boost::uuids::random_generator()());
		updateActiveTime();
	}

	virtual ~connection() { disconnect(); }

public:
	// 设置元数据 [Only 业务层] [Not Thread Safe]
	template <class ValueType>
	void setMetaData(const std::string& key, ValueType value) {
		m_meta[key] = value;
	}

	// 获取元数据 [Only 业务层] [Not Thread Safe]
	template <class ValueType>
	std::optional<ValueType> getMetaData(const std::string& key) {
		if (auto it = m_meta.find(key); it != m_meta.end()) {
			return std::any_cast<ValueType>(it->second);
		}

		return std::nullopt;
	}

	// 移除元数据 [Only 业务层] [Not Thread Safe]
	void removeMetaData(const std::string& key) { m_meta.erase(key); }

	// 异步处理消息
	void ayncHandling() {
		if (m_socket.is_open()) {
			// 监听 读事件
			readHeader();
		}
	}

	// io_context线程中关闭socket
	void disconnect() {
		if (isConnected())
			net::post(m_socket.get_executor(), [this]() {
				m_socket.close();
				m_server.onClientDisconnect(this->shared_from_this());
			});
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

	// 获取连接标识
	std::string getId() const { return m_uuid; }

	// 获取最后活跃时间
	std::chrono::steady_clock::time_point getLastActiveTime() const { return m_last_time; }

	// 更新最后活跃时间
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

								disconnect();
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

								disconnect();
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

								 disconnect();
								 return;
							 }

							 //  fmt::println("writeHeader:{}, write {} bytes", __LINE__, length);

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

								 disconnect();
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
		auto msg = std::make_shared<owned_message<T>>(this->shared_from_this(), m_tmp_msg_in);
		m_mq_recv.push_back(msg);

		// 更新最后活跃时间
		updateActiveTime();
	}

protected:
	net::io_context& m_ctx;

	// 当前连接的 owner server
	server_interface<T>& m_server;

	tcp::socket m_socket;

	// 临时 保存接收消息
	Message<T> m_tmp_msg_in;

	// 消息发送队列
	tsqueue<Message<T>> m_mq_send;
	// 消息接收队列
	tsqueue<owned_message<T>>& m_mq_recv;

	// 标识 TCP 连接
	std::string m_uuid;
	// 最后活跃时间,用于心跳检测
	std::chrono::steady_clock::time_point m_last_time;

	// 元数据
	std::map<std::string, std::any> m_meta;
};
