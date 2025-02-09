#pragma once

#include <qjsonobject.h>
#include <qtmetamacros.h>
#include "Singleton.hpp"
#include "Common.h"

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>

class HttpManager : public QObject, public Singleton<HttpManager> {
	Q_OBJECT

	// 允许Singleton类 访问HttpManager私有构造函数
	friend class Singleton<HttpManager>;

public:
	~HttpManager() override = default;

	// 发送GET请求
	void post(const QUrl& url, const QJsonObject& json, ModuleType module_type,
			  RequestType request_type);

private:
	// 派发信号
	void dispatchSingnal(ModuleType module_type, RequestType request_type, const QJsonObject& json);
signals:
	// 注册模块信号
	void sig_module_register_finished(RequestType request_type, const QJsonObject& json);
	// 登录模块信号
	void sig_module_login_finished(RequestType request_type, const QJsonObject& json);

private:
	QNetworkAccessManager* m_net_manager;

private:
	explicit HttpManager(QObject* parent = nullptr);
};