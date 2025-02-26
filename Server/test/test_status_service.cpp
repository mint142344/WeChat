#include "RpcService.h"
#include "pool/RpcConnPool.hpp"
#include <gtest/gtest.h>
#include <thread>

class RpcServiceTest : public ::testing::Test {
protected:
	void SetUp() override {
		// 确保状态服务器已经启动, 初始化连接池
		// ./StatusServer > txt
		RpcServiceConnPool<StatusService>::getInstance()->init("127.0.0.1", 6001, 4);
	}
};

// 测试多个用户 并发获取 ChatServerInfo 登录ChatServer
TEST_F(RpcServiceTest, GetChatServerInfoMulti) {
	std::vector<std::thread> threads;
	size_t user_count = 200;
	threads.reserve(user_count);

	for (uint32_t i = 0; i < user_count; ++i) {
		threads.emplace_back([i] {
			json result = RPC::getChatServerInfo(i);
			ASSERT_EQ(result["status"], "ok") << result["message"];
			ASSERT_TRUE(result.contains("host"));
			ASSERT_TRUE(result.contains("port"));
			ASSERT_TRUE(result.contains("token"));
			ASSERT_TRUE(result.contains("message"));

			// 使用获取的token登录
			result = RPC::loginChatServer(i, result["token"]);
			ASSERT_EQ(result["status"], "ok");
		});
	}

	for (auto& t : threads) {
		t.join();
	}
}