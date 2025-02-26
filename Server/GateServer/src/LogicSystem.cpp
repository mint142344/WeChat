#include "LogicSystem.h"
#include "DAO/ErrorCode.h"
#include "RpcService.h"
#include "pool/RedisConnPool.h"
#include "MysqlService.h"

#include <fmt/base.h>
#include <boost/beast/core/ostream.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

constexpr const char* keyPrefix = "email_verify_code:";

HttpResponse ErrorResponse::operator()(http::status status, const std::string& err_msg) {
	HttpResponse res{status, 11};
	res.set(http::field::content_type, "application/json");
	res.set(http::field::server, "Beast GateServer");

	json data = {{"status", "error"}, {"message", err_msg}};
	beast::ostream(res.body()) << data.dump();
	res.payload_size();

	return res;
}

void LogicSystem::init() {
	// GET /
	registerRoute("/", http::verb::get, [](const HttpRequest& req, HttpResponse& res) {
		res.set(http::field::content_type, "application/json");
		json data = {{"status", "OK"}, {"message", "Hello World"}};

		beast::ostream(res.body()) << data.dump();

		res.result(http::status::ok);
		res.prepare_payload();
	});

	// POST /verify_code	获取邮箱验证码
	registerRoute("/verify_code", http::verb::post, [](const HttpRequest& req, HttpResponse& res) {
		json data;
		try {
			json req_json = json::parse(req.body());

			// invalid argument
			if (!req_json.contains("email")) {
				res = ErrorResponse{}(http::status::bad_request, "Required 'email' argument");
				return;
			}

			// RPC call
			json data = RPC::getEmailVerifyCode(req_json["email"]);
			if (data["status"] == "ok") {
				res.result(http::status::ok);
			} else {
				res.result(http::status::internal_server_error);
			}

			res.set(http::field::content_type, "application/json");
			beast::ostream(res.body()) << data.dump();
			res.prepare_payload();

		} catch (const std::exception& e) {
			// json parse failed
			res = ErrorResponse{}(http::status::bad_request, "Incorrect request format");
			return;
		}
	});

	// POST /register	注册
	registerRoute("/register", http::verb::post, [](const HttpRequest& req, HttpResponse& res) {
		fmt::println("post /register");
		json data;

		try {
			json req_json = json::parse(req.body());

			// 设置响应头
			res.result(http::status::ok);
			res.set(http::field::content_type, "application/json");

			// 检查验证码
			RedisConnGuard redis_guard(RedisConnPool::getInstance()->getConnection());
			std::string verify_code;
			std::string key = keyPrefix + req_json["email"].get<std::string>();

			// 验证码错误
			if (!redis_guard->get(key, verify_code) ||
				req_json["verify_code"].get<std::string>() != verify_code) {
				data = {{"status", "error"}, {"message", "验证码错误"}};
				beast::ostream(res.body()) << data.dump();
				res.prepare_payload();
				return;
			}

			// 数据库 注册用户
			ErrorCode ec = MysqlService::getInstance()->registerUser(
				req_json["username"].get<std::string>(), req_json["email"].get<std::string>(),
				req_json["password"].get<std::string>());

			if (ec == ErrorCode::OK) {
				data = {{"status", "ok"}, {"message", "注册成功"}};
				// 删除 redis 缓存
				redis_guard->del(key.c_str());

			} else if (ec == ErrorCode::ALREADY_EXISTS) {
				data = {{"status", "error"}, {"message", "注册失败，该用户已存在"}};

			} else {
				data = {{"status", "error"}, {"message", "注册失败，服务器内部错误"}};
			}

			beast::ostream(res.body()) << data.dump();
			res.prepare_payload();

		} catch (const std::exception& e) {
			// json parse failed
			res = ErrorResponse{}(http::status::bad_request, "Incorrect request format");
			return;
		}
	});

	// POST /forget_password	忘记密码(修改密码)
	registerRoute(
		"/forget_password", http::verb::post, [](const HttpRequest& req, HttpResponse& res) {
			fmt::println("post /forget_password");
			json data;

			try {
				json req_json = json::parse(req.body());

				// 设置响应头
				res.result(http::status::ok);
				res.set(http::field::content_type, "application/json");

				// 检查验证码
				RedisConnGuard redis_guard(RedisConnPool::getInstance()->getConnection());
				std::string verify_code;
				std::string key = keyPrefix + req_json["email"].get<std::string>();

				// 验证码错误
				if (!redis_guard->get(key, verify_code) ||
					req_json["verify_code"].get<std::string>() != verify_code) {
					data = {{"status", "error"}, {"message", "验证码错误"}};
					beast::ostream(res.body()) << data.dump();
					res.prepare_payload();
					return;
				}

				// 数据库 修改密码
				ErrorCode ec = MysqlService::getInstance()->modifyPasswd(
					req_json["username"].get<std::string>(),
					req_json["password"].get<std::string>());

				if (ec == ErrorCode::OK) {
					data = {{"status", "ok"}, {"message", "修改密码成功"}};
					// 删除 redis 缓存
					redis_guard->del(key.c_str());

				} else if (ec == ErrorCode::NOT_FOUND) {
					data = {{"status", "error"}, {"message", "修改密码失败,清检查用户名是否正确"}};
				} else {
					data = {{"status", "error"}, {"message", "修改密码失败，服务器内部错误"}};
				}

				beast::ostream(res.body()) << data.dump();
				res.prepare_payload();
			} catch (const std::exception& e) {
				// json parse failed
				res = ErrorResponse{}(http::status::bad_request, "Incorrect request format");
				return;
			}
		});

	// POST /login	登录
	registerRoute("/login", http::verb::post, [](const HttpRequest& rep, HttpResponse& res) {
		fmt::println("post /login");
		json data;

		try {
			json req_json = json::parse(rep.body());

			// 设置响应头
			res.result(http::status::ok);
			res.set(http::field::content_type, "application/json");

			// 数据库 登录
			ErrorCode ec = MysqlService::getInstance()->login(
				req_json["username"].get<std::string>(), req_json["password"].get<std::string>());

			if (ec == ErrorCode::OK) {
				data = {{"status", "ok"}, {"message", "登录成功"}};
			} else if (ec == ErrorCode::DB_ERROR) {
				data = {{"status", "error"}, {"message", "登录失败，服务器内部错误"}};
			} else {
				data = {{"status", "error"}, {"message", "用户名或密码错误"}};
			}

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

void LogicSystem::registerRoute(const std::string& route, http::verb method,
								const RouteHandler& handler) {
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
