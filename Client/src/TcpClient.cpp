#include "TcpClient.h"
#include <QJsonObject>
#include <QJsonDocument>

void TcpClient::sendMsg(MsgID id, const QByteArray& body) {
	message_header<MsgID> header;
	header.id = id;
	header.size = body.size();

	m_socket->write((char*)(&header), sizeof(header));
	m_socket->write(body);

	// 已发送消息
	emit sentMsg(id);
}

void TcpClient::loginToHost(const QString& host, uint16_t port) {
	m_socket->connectToHost(host, port);
}

TcpClient::TcpClient(QObject* parent) : m_socket{new QTcpSocket{parent}} {
	registerCallback();

	connect(m_socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
	connect(m_socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
	connect(m_socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
	connect(m_socket, &QTcpSocket::errorOccurred, this, &TcpClient::onError);
	connect(m_socket, &QTcpSocket::stateChanged, this, &TcpClient::onStateChanged);
}

void TcpClient::onConnected() {
	qDebug() << "Connected to " << m_socket->peerAddress() << ":" << m_socket->peerPort();
}

void TcpClient::onDisconnected() {
	qDebug() << "Disconnected from " << m_socket->peerAddress() << ":" << m_socket->peerPort();

	m_socket->setProperty("id", 0);
	m_socket->setProperty("token", "");

	emit disconnectedChatServer();
}

void TcpClient::onReadyRead() {
	m_buffer.append(m_socket->readAll());

	// 1.read header
	if (m_buffer.size() < sizeof(message_header<MsgID>)) {
		return;
	}

	if (m_state == ReadState::HEADER) {
		memcpy(&m_header, m_buffer.data(), sizeof(message_header<MsgID>));
		m_buffer.remove(0, sizeof(message_header<MsgID>));

		m_state = ReadState::BODY;
	}

	// 2.read body
	if (m_buffer.size() < m_header.size) {
		return;
	}

	QByteArray body = m_buffer.left(m_header.size);
	m_buffer.remove(0, m_header.size);

	// 处理消息
	handleMessage(m_header.id, body);

	// continue read
	m_state = ReadState::HEADER;
}

void TcpClient::onStateChanged(QAbstractSocket::SocketState socketState) {
	qDebug() << "TcpClient::onStateChanged: " << socketState;

	// 未连接 未认证
	if (socketState == QAbstractSocket::UnconnectedState && !m_auth) {
		emit loggedChatServer(false, "连接聊天服务器失败");
	}

	// 已连接 ChatServer,未认证
	if (socketState == QAbstractSocket::ConnectedState && !m_auth) {
		loginChatServer();
	}
}

void TcpClient::onError(QAbstractSocket::SocketError socketError) {
	qDebug() << "TcpClient::onError: " << socketError << m_socket->errorString();
}

void TcpClient::registerCallback() {
	// 登录 ChatServer
	m_callbacks[MsgID::LOGIN_CHAT_SERVER_ACK] = [this](const QByteArray& body) {
		qDebug() << "LOGIN_CHAT_SERVER_ACK: " << body.size();

		QJsonObject json = QJsonDocument::fromJson(body).object();

		if (json["status"].toString() == "ok") {
			m_auth = true;
			emit loggedChatServer(true);
		} else {
			m_auth = false;
			emit loggedChatServer(false, "无法连接到聊天服务器");
		}
	};
}

void TcpClient::handleMessage(MsgID id, const QByteArray& body) {
	qDebug() << "handleMessage: " << enum_to_string(id);
	m_callbacks[id](body);
}

void TcpClient::loginChatServer() {
	if (m_auth) return;

	QJsonObject json;
	json["id"] = m_socket->property("id").toInt();
	json["token"] = m_socket->property("token").toString();

	sendMsg(MsgID::LOGIN_CHAT_SERVER, QJsonDocument(json).toJson(QJsonDocument::Compact));
}
