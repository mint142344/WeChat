#pragma once

#include "Singleton.hpp"
#include "Common.h"

#include <QtNetwork/QNetworkAccessManager>

class HttpManager : public QObject, public Singleton<HttpManager> {
	Q_OBJECT

	// 允许Singleton类 访问HttpManager私有构造函数
	friend class Singleton<HttpManager>;

public:
	~HttpManager() override = default;

	// 发送GET请求
	void post(const QString& route, const QJsonObject& json, ModuleType module_type,
			  RequestType request_type);

private:
	// 派发信号
	void dispatchSingnal(ModuleType module_type, RequestType request_type, const QJsonObject& json);
signals:
	// 用户模块(注册/找回密码)信号
	void sig_module_user_finished(RequestType request_type, const QJsonObject& json);
	// 登录模块信号
	void sig_module_login_finished(RequestType request_type, const QJsonObject& json);

private:
	QNetworkAccessManager* m_net_manager;
	QString m_url_prefix;

private:
	explicit HttpManager(QObject* parent = nullptr);
};