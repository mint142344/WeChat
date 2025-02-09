#include "Listener.h"

#include <fmt/base.h>
#include <csignal>
#include <memory>

int main(int argc, char* argv[]) {
	net::io_context ioc;
	net::signal_set signals(ioc, SIGINT, SIGTERM);

	try {
		signals.async_wait([&ioc](const std::error_code& ec, int signal) {
			ioc.stop();
			fmt::println("Interrupted by a signal {}", signal);
		});

		tcp::endpoint endpoint(tcp::v4(), 5000);
		std::make_shared<Listener>(ioc, endpoint)->start();

		ioc.run();

	} catch (std::exception& e) {
		fmt::println("Exception: {}", e.what());
		return 1;
	}

	ioc.run();

	return 0;
}