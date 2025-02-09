#include "HttpManager.h"
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstringview.h>

#include <QJsonObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

HttpManager::HttpManager(QObject* parent) : QObject{parent}, Singleton{}, m_net_manager{} {}

void HttpManager::post(const QUrl& url, const QJsonObject& json, ModuleType module_type,
					   RequestType request_type) {
	// 创建请求 with header
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QByteArray data = QJsonDocument(json).toJson();
	request.setHeader(QNetworkRequest::ContentLengthHeader, data.length());

	// 发送请求
	QNetworkReply* reply = m_net_manager->post(request, data);

	// 异步处理
	connect(reply, &QNetworkReply::finished, [this, reply, module_type, request_type]() {
		reply->deleteLater();

		QByteArray response = reply->readAll();
		QJsonObject obj = QJsonDocument::fromJson(response).object();

		if (reply->error() != QNetworkReply::NoError) {
			qDebug() << "HttpManager::post error: " << reply->errorString();
		}

		// 派发信号 给对应的模块
		this->dispatchSingnal(module_type, request_type, obj);
	});
}

void HttpManager::dispatchSingnal(ModuleType module_type, RequestType request_type,
								  const QJsonObject& json) {
	switch (module_type) {
		case ModuleType::REGISTER:
			emit sig_module_register_finished(request_type, json);
			break;
		case ModuleType::LOGIN:
			emit sig_module_login_finished(request_type, json);
			break;
	}
}