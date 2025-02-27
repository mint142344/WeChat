#include "GateServer/include/RpcService.h"
#include "ChatServer/include/RpcService.h"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>

class RpcServiceTest : public ::testing::Test {
protected:
	void SetUp() override {
		// 确保状态服务器已经启动
		RpcServiceConnPool<StatusService>::getInstance()->init("127.0.0.1", 6001, 4);
	}
};

// 测试多个用户 并发获取 ChatServerInfo 并 ChatServer验证登录
TEST_F(RpcServiceTest, GetChatServerInfoMulti) {
	std::vector<std::thread> threads;
	size_t user_count = 200;
	threads.reserve(user_count);

	for (uint32_t i = 0; i < user_count; ++i) {
		threads.emplace_back([i] {
			// 1.客户端获取 ChatServerInfo
			json info = RPC::getChatServerInfo(i);
			ASSERT_EQ(info["status"], "ok");
			ASSERT_TRUE(info.contains("host"));
			ASSERT_TRUE(info.contains("port"));
			ASSERT_TRUE(info.contains("token"));
			ASSERT_TRUE(info.contains("message"));

			// 2.客户使用错误的 token 登录
			json res2 = RPC::verifyToken(i, "error_token");
			ASSERT_NE(res2["status"], "ok");

			// 3.验证失败清除 token (用户需再次重新登录)
			res2 = RPC::verifyToken(i, info["token"]);
			ASSERT_NE(res2["status"], "ok");

			// 模拟重新登录
			std::this_thread::sleep_for(std::chrono::seconds{2});

			// 4.客户端再次获取 ChatServerInfo
			info = RPC::getChatServerInfo(i);
			ASSERT_EQ(info["status"], "ok");
			ASSERT_TRUE(info.contains("host"));
			ASSERT_TRUE(info.contains("port"));
			ASSERT_TRUE(info.contains("token"));
			ASSERT_TRUE(info.contains("message"));

			// 5.客户端使用正确的 token 登录
			res2 = RPC::verifyToken(i, info["token"]);
			ASSERT_EQ(res2["status"], "ok");

			// 6.成功登录后 禁止再次登录
			info = RPC::getChatServerInfo(i);
			ASSERT_NE(info["status"], "ok");
		});
	}

	for (auto& t : threads) {
		t.join();
	}
}

// 测试 StatuServer 定时清理 超时的未验证用户
TEST_F(RpcServiceTest, CleanJunkUsers) {
	std::vector<std::thread> threads;
	size_t user_count = 200;
	threads.reserve(user_count);

	for (uint32_t i = 0; i < user_count; ++i) {
		threads.emplace_back([i]() {
			// 1.客户端获取 ChatServerInfo
			uint32_t id = 1000 + i;
			json info = RPC::getChatServerInfo(id);
			ASSERT_EQ(info["status"], "ok");
			ASSERT_TRUE(info.contains("host"));
			ASSERT_TRUE(info.contains("port"));
			ASSERT_TRUE(info.contains("token"));
			ASSERT_TRUE(info.contains("message"));

			// 模拟网络延迟
			std::this_thread::sleep_for(std::chrono::seconds{20});

			// 2.客户端登录(超时)
			json res2 = RPC::verifyToken(id, info["token"]);
			ASSERT_NE(res2["status"], "ok");

			// 3.重新登录
			info = RPC::getChatServerInfo(id);
			ASSERT_EQ(info["status"], "ok");
			res2 = RPC::verifyToken(id, info["token"]);
			ASSERT_EQ(res2["status"], "ok");
		});
	}

	for (auto& t : threads) {
		t.join();
	}
}