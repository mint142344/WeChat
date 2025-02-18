#include "HttpManager.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QStandardPaths>

HttpManager::HttpManager(QObject* parent)
	: QObject{parent}, m_net_manager{new QNetworkAccessManager(this)} {
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/wechat_config.ini";
	m_net_manager->setTransferTimeout(10000);

	// 配置文件
	QSettings settings(path, QSettings::Format::IniFormat);
	QString host = settings.value("GateServer/host", "").toString();
	uint16_t port = settings.value("GateServer/port", 0).toUInt();
	if (host.isEmpty() || port == 0) {
		host = "localhost";
		port = 8080;
		settings.setValue("GateServer/host", host);
		settings.setValue("GateServer/port", port);
		settings.sync();
	}
	m_url_prefix = QString("http://%1:%2").arg(host).arg(port);
	// qDebug() << __PRETTY_FUNCTION__ << __LINE__ << path << m_url_prefix;
}

void HttpManager::post(const QString& route, const QJsonObject& json, ModuleType module_type,
					   RequestType request_type) {
	// qDebug() << __PRETTY_FUNCTION__ << __LINE__ << m_url_prefix + route;

	// 创建请求 with header
	QNetworkRequest request(m_url_prefix + route);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray data = QJsonDocument(json).toJson();
	request.setHeader(QNetworkRequest::ContentLengthHeader, data.length());

	// 发送请求
	QNetworkReply* reply = m_net_manager->post(request, data);

	connect(reply, &QNetworkReply::errorOccurred, [this, reply]() {
		qDebug() << "[QNetworkReply::errorOccurred]" << reply->errorString();
	});

	// 异步处理
	connect(reply, &QNetworkReply::finished, [this, reply, module_type, request_type]() {
		reply->deleteLater();

		QByteArray response = reply->readAll();
		QJsonObject obj = QJsonDocument::fromJson(response).object();

		if (reply->error() != QNetworkReply::NoError) {
			qDebug() << "[QNetworkReply::finished]" << reply->errorString();

			obj["status"] = "error";

			switch (reply->error()) {
				case QNetworkReply::HostNotFoundError:
					obj["message"] = "服务器未找到";
					break;
				case QNetworkReply::OperationCanceledError:
				case QNetworkReply::TimeoutError:
					obj["message"] = "请求超时";
					break;
				case QNetworkReply::ConnectionRefusedError:
					obj["message"] = "连接被拒绝";
					break;
				case QNetworkReply::RemoteHostClosedError:
					obj["message"] = "远程主机关闭连接";
					break;
				case QNetworkReply::InternalServerError:
					obj["message"] = "服务器内部错误";
					break;
				default:
					obj["message"] = reply->errorString();
					break;
			}
		}

		// 派发信号 给对应的模块
		this->dispatchSingnal(module_type, request_type, obj);
	});
}

void HttpManager::dispatchSingnal(ModuleType module_type, RequestType request_type,
								  const QJsonObject& json) {
	switch (module_type) {
		case ModuleType::USER:
			emit sig_module_user_finished(request_type, json);
			break;
		case ModuleType::LOGIN:
			emit sig_module_login_finished(request_type, json);
			break;
	}
}