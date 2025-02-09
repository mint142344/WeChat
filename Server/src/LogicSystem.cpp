#include "LogicSystem.h"
#include <fmt/base.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

HttpResponse ErrorResponse::operator()(http::status status, const std::string& err_msg) {
	HttpResponse res{status, 11};
	res.set(http::field::content_type, "application/json");
	res.set(http::field::server, "Beast GateServer");

	json data = {{"status", "error"}, {"message", err_msg}};
	beast::ostream(res.body()) << data.dump();
	res.payload_size();

	return res;
}

LogicSystem::LogicSystem() {
	// GET /
	registerRoute("/", http::verb::get, [](const HttpRequest& req, HttpResponse& res) {
		res.set(http::field::content_type, "application/json");
		json data = {{"status", "OK"}, {"message", "Hello World"}};

		beast::ostream(res.body()) << data.dump();

		res.result(http::status::ok);
		res.prepare_payload();
	});

	// POST /verify_code
	registerRoute(
		"/verify_code", http::verb::post, [this](const HttpRequest& req, HttpResponse& res) {
			json data;
			try {
				json req_json = json::parse(req.body());

				// invalid argument
				if (!req_json.contains("email")) {
					res = ErrorResponse{}(http::status::bad_request, "Required 'email' argument");
					return;
				}

				res.result(http::status::ok);
				res.set(http::field::content_type, "application/json");

				data = {{"status", "OK"}, {"email", req_json["email"]}};
				beast::ostream(res.body()) << data.dump();
				res.prepare_payload();

			} catch (const std::exception& e) {
				// json parse failed
				res = ErrorResponse{}(http::status::bad_request, "Incorrect request format");
				return;
			}
		});
}

bool LogicSystem::handleRequest(const std::string& route, const HttpRequest& req,
								HttpResponse& res) {
	res.set(http::field::server, "Beast GateServer");
	// route= "/你好"
	// route:/%e4%bd%a0%e5%a5%bd, size:19

	switch (req.method()) {
		// GET
		case http::verb::get: {
			auto it = m_get_handlers.find(route);
			// 查找路由
			if (it == m_get_handlers.end()) {
				res.result(http::status::not_found);
				res.set(http::field::content_type, "txt/plain");

				beast::ostream(res.body()) << "Not Found";
				res.prepare_payload();
				return false;
			}

			// 处理请求
			m_get_handlers[route](req, res);
			return true;
		} break;

		// POST
		case http::verb::post: {
			auto it = m_post_handlers.find(route);
			// 查找路由
			if (it == m_post_handlers.end()) {
				res.result(http::status::not_found);
				res.set(http::field::content_type, "txt/plain");

				beast::ostream(res.body()) << "Not Found";
				res.prepare_payload();
				return false;
			}

			// 处理请求
			m_post_handlers[route](req, res);
			return true;
		} break;

		default:
			fmt::println("Unsupported method:{}", req.method_string());
			break;
	}
	return false;
}

void LogicSystem::registerRoute(const std::string& route, http::verb method, RouteHandler handler) {
	switch (method) {
		case http::verb::get:
			m_get_handlers.insert({route, handler});
			break;
		case http::verb::post:
			m_post_handlers.insert({route, handler});
			break;
		default:
			break;
	}
}
