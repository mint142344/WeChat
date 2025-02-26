#include "StatusServer.h"

#include <boost/asio.hpp>
#include <csignal>
#include <fmt/base.h>
#include <fmt/format.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include <string>
#include <fstream>

using namespace boost;

int main(int argc, char* argv[]) {
	// argc argv 修改读取配置文件路径

	// 1.读取配置文件，初始化 ChatServer 信息
	std::ifstream ifs("status_server_config.json");
	if (!ifs) {
		throw std::runtime_error("status_server_config.json not found");
	}

	json data = json::parse(ifs);
	if (!checkStatusServerConfig(data)) {
		throw std::runtime_error("please refer to the format below\n" +
								 DEFAULT_STATUS_CONFIG.dump(4));
	}

	// StatusServer 端点
	std::string endpoint = data["StatusServer"]["ip"].get<std::string>() + ":" +
						   std::to_string(data["StatusServer"]["port"].get<uint16_t>());

	asio::io_context ioc;
	asio::signal_set sig_set(ioc, SIGINT, SIGTERM);

	// 2.构建 StatusServer
	StatusServiceImpl status_server(data);
	ServerBuilder builder;
	builder.AddListeningPort(endpoint, grpc::InsecureServerCredentials());
	builder.RegisterService(&status_server);

	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

	sig_set.async_wait([&server](const std::error_code& ec, int signo) {
		if (ec) {
			fmt::println("Signal = {}, Error: {}", signo, ec.message());
			return;
		}
		// 关闭 StatusServer
		server->Shutdown();
		fmt::println("Interrupted by a signal {}", signo);
	});

	std::thread t([&ioc]() { ioc.run(); });

	fmt::println("Listening on {}", endpoint);
	server->Wait();

	t.join();

	return 0;
}