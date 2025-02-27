#pragma once
#include "Common/net.h"
#include "Common/Singleton.hpp"

#include <functional>
#include <map>
#include <string>

using HttpRequest = http::request<http::string_body>;
using HttpResponse = http::response<http::dynamic_body>;
using RouteHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

// 辅助函数对象 生成一个错误响应
struct ErrorResponse {
	HttpResponse operator()(http::status status, const std::string& err_msg);
};

class LogicSystem : public Singleton<LogicSystem> {
	friend class Singleton<LogicSystem>;

public:
	LogicSystem() = default;
	~LogicSystem() = default;
	LogicSystem(const LogicSystem&) = delete;
	LogicSystem(LogicSystem&&) = delete;
	LogicSystem& operator=(const LogicSystem&) = delete;
	LogicSystem& operator=(LogicSystem&&) = delete;

	// 初始化所有请求处理函数
	void init();

	// 处理 GET/POST 请求
	bool handleRequest(const std::string& route, const HttpRequest& req, HttpResponse& res);

private:
	// 注册路由
	void registerRoute(const std::string& route, http::verb method, const RouteHandler& handler);

private:
	std::map<std::string, RouteHandler> m_get_handlers;
	std::map<std::string, RouteHandler> m_post_handlers;
};
