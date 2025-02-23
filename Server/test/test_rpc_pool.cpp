#include "pool/RpcConnPool.h"
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

class RpcPoolTest : public ::testing::Test {
protected:
	// SetUp 方法在每个测试用例运行之前自动调用，用于进行测试前的初始化工作。
	void SetUp() override {
		// Initialize pool with test configuration
		std::cout << "RpcPoolTest::SetUp" << '\n';
		RpcServiceConnPool<EmailVerifyService>::getInstance()->init("localhost", 5001, 4);
	}

	// 每个测试用例运行之后自动调用，用于进行测试后的清理工作。
	void TearDown() override {
		// Clean up will happen automatically via RAII
	}
};

// 测试一次 RPC 连接
TEST_F(RpcPoolTest, BasicGetConnection) {
	size_t poll_size = RpcServiceConnPool<EmailVerifyService>::getInstance()->size();
	// 获取一个空闲的 Stub
	auto stub = RpcServiceConnPool<EmailVerifyService>::getInstance()->getConnection();
	ASSERT_NE(stub, nullptr);
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(), poll_size - 1);

	ClientContext context;
	EmailVerifyRequest request;
	request.set_email("1423443710@qq.com");

	EmailVerifyResponse response;

	Status ret = stub->getEmailVerifyCode(&context, request, &response);
	EXPECT_TRUE(ret.ok()) << "Error: " << ret.error_message();

	// 归还 Stub
	RpcServiceConnPool<EmailVerifyService>::getInstance()->releaseConnection(std::move(stub));
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(), poll_size);
}

// 测试多线程并发访问
TEST_F(RpcPoolTest, ConcurrentAccess) {
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(),
			  RpcServiceConnPool<EmailVerifyService>::getInstance()->size());

	constexpr int NUM_THREADS = 100;
	std::vector<std::thread> threads;

	threads.reserve(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back([]() {
			// 3s的超时等待
			auto stub = RpcServiceConnPool<EmailVerifyService>::getInstance()->getConnection(
				std::chrono::seconds{1});
			if (stub) {
				// Simulate work
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				RpcServiceConnPool<EmailVerifyService>::getInstance()->releaseConnection(
					std::move(stub));
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}

	// All threads should eventually get a connection
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(),
			  RpcServiceConnPool<EmailVerifyService>::getInstance()->size());
}
