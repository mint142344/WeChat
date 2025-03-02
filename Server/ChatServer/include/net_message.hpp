#pragma once

#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>

#include "../message.h"

// 存储要发送的消息
template <class T>
struct Message {
private:
	MessageHeader<T> m_header{};
	std::vector<uint8_t> m_body;

public:
	Message() {
		static_assert(std::is_enum_v<T>, "T must be enum class");
		m_body.reserve(1024);
	}

	Message(T id) : m_header({id, 0}) {
		static_assert(std::is_enum_v<T>, "T must be enum class");
		m_body.reserve(1024);
	}

public:
	void setId(T id) { m_header.id = id; }

	T getId() const { return m_header.id; }

	char* header() { return (char*)&m_header; }

	char* data() { return (char*)m_body.data(); }

	// body size
	size_t size() const { return m_header.size; }

	void resizeBody(size_t size) { m_body.resize(size); }

public:
	// Note:POD data, push any trivial data to message
	template <class DataType>
	friend Message<T>& operator<<(Message& msg, const DataType& data) {
		static_assert(std::is_standard_layout_v<DataType>,
					  "Data is too complex to be pushed into vector");

		size_t old_size = msg.m_body.size();
		size_t data_size = sizeof(DataType);
		msg.m_body.resize(old_size + data_size);

		std::memcpy(msg.m_body.data() + old_size, &data, data_size);
		msg.m_header.size = msg.m_body.size();

		return msg;
	}

	// Note:POD data, pop any trivial data from message
	template <class DataType>
	friend Message<T>& operator>>(Message& msg, DataType& data) {
		static_assert(std::is_standard_layout<DataType>::value,
					  "Data is too complex to be pushed into vector");

		size_t old_size = msg.m_body.size();
		size_t data_size = sizeof(DataType);

		if (old_size < data_size) {
			throw std::out_of_range("message body is too small");
		}

		std::memcpy(&data, msg.m_body.data() + old_size - data_size, data_size);
		// 不改变Capacity 不影响性能
		msg.m_body.resize(old_size - data_size);
		msg.m_header.size = msg.m_body.size();

		return msg;
	}

	void writeBody(const char* data, size_t size) {
		if (data == nullptr || size == 0) return;

		m_body.insert(m_body.end(), data, data + size);

		m_header.size = m_body.size();
	}

	void readBody(char* data, size_t size) {
		if (data == nullptr || size == 0) {
			throw std::invalid_argument("data is nullptr or size is 0");
		}

		if (m_body.empty()) {
			throw std::out_of_range("message body is empty");
		}

		size_t copy_size = std::min(m_body.size(), size);
		std::memcpy(data, m_body.data(), copy_size);
		m_body.erase(m_body.begin(), m_body.begin() + copy_size);

		m_header.size = m_body.size();
	}

	// 打印消息
	friend std::ostream& operator<<(std::ostream& os, const Message<T>& msg) {
		// os << "ID:" << enum_to_string(msg.getId()) << " body size:" << msg.m_body.size()
		//    << ", body:" << msg.m_body.data();

		os << "ID:" << enum_to_string(msg.getId()) << " body size:" << msg.m_body.size();

		return os;
	}
};

// 前置声明
template <class T>
class connection;

// 存储接收消息 (peer + msg)
template <typename T>
struct owned_message {
public:
	friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg) {
		os << msg.msg;
		return os;
	}

	owned_message(std::shared_ptr<connection<T>> remote, const Message<T>& msg)
		: peer(remote), msg(msg) {}

public:
	std::shared_ptr<connection<T>> peer; // only server: 哪个client发送的消息
	Message<T> msg;
};

template <class T>
using RcvMsgPtr = std::shared_ptr<owned_message<T>>;

template <class T>
using SndMsgPtr = std::shared_ptr<Message<T>>;

template <class T>
SndMsgPtr<T> makeSndMsg(T id) {
	return std::make_shared<Message<T>>(id);
}
