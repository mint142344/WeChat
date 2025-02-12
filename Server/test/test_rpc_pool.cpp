#include "EmailVerifyClient.h"
#include "ConfigManger.h"

#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>

class RpcPoolTest : public ::testing::Test {
protected:
	// SetUp 方法在每个测试用例运行之前自动调用，用于进行测试前的初始化工作。
	void SetUp() override {
		// Initialize pool with test configuration
		std::cout << "RpcPoolTest::SetUp" << std::endl;
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
	auto stub = RpcServiceConnPool<EmailVerifyService>::getInstance()->get();
	ASSERT_NE(stub, nullptr);
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(), poll_size - 1);

	ClientContext context;
	EmailVerifyRequest request;
	request.set_email("1423443710@qq.com");

	EmailVerifyResponse response;

	Status ret = stub->getEmailVerifyCode(&context, request, &response);
	EXPECT_TRUE(ret.ok()) << "Error: " << ret.error_message();

	// 归还 Stub
	RpcServiceConnPool<EmailVerifyService>::getInstance()->put(std::move(stub));
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(), poll_size);
}

// 测试多线程并发访问
TEST_F(RpcPoolTest, ConcurrentAccess) {
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(),
			  RpcServiceConnPool<EmailVerifyService>::getInstance()->size());
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->busy(), 0);

	constexpr int NUM_THREADS = 100;
	std::vector<std::thread> threads;

	for (int i = 0; i < NUM_THREADS; ++i) {
		threads.emplace_back([]() {
			// 3s的超时等待
			auto stub =
				RpcServiceConnPool<EmailVerifyService>::getInstance()->get(std::chrono::seconds{1});
			if (stub) {
				// Simulate work
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				RpcServiceConnPool<EmailVerifyService>::getInstance()->put(std::move(stub));
			}
		});
	}

	for (auto& t : threads) {
		t.join();
	}

	// All threads should eventually get a connection
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->available(),
			  RpcServiceConnPool<EmailVerifyService>::getInstance()->size());
	EXPECT_EQ(RpcServiceConnPool<EmailVerifyService>::getInstance()->busy(), 0);
}

// 测试连接复用，都访问vector的第一个元素
TEST_F(RpcPoolTest, ConnectionReuse) {
	auto stub1 = RpcServiceConnPool<EmailVerifyService>::getInstance()->get();
	auto stub1_ptr = stub1.get(); // Store raw pointer for comparison
	RpcServiceConnPool<EmailVerifyService>::getInstance()->put(std::move(stub1));

	auto stub2 = RpcServiceConnPool<EmailVerifyService>::getInstance()->get();
	EXPECT_EQ(stub2.get(), stub1_ptr); // Should get the same connection
}