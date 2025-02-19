#include "pool/IoContextPool.h"
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

class IoContextPoolTest : public ::testing::Test {
protected:
	void SetUp() override {
		// 在每个测试用例开始前执行
		IoContextPool::getInstance()->start(4);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	void TearDown() override {
		// 在每个测试用例结束后执行
		IoContextPool::getInstance()->stop();
	}
};

// 测试基本的轮询分配功能
TEST_F(IoContextPoolTest, RoundRobinTest) {
	auto* pool = IoContextPool::getInstance();

	auto& ctx1 = pool->getIoContext();
	auto& ctx2 = pool->getIoContext();
	auto& ctx3 = pool->getIoContext();
	auto& ctx4 = pool->getIoContext();
	auto& ctx5 = pool->getIoContext();

	EXPECT_NE(&ctx1, &ctx2); // 第一个和第二个应该不同
	EXPECT_NE(&ctx2, &ctx3);
	EXPECT_NE(&ctx3, &ctx4);
	EXPECT_EQ(&ctx5, &ctx1);
	std::cout << "RoundRobinTest" << "\n";
}

// 测试工作负载分布
TEST_F(IoContextPoolTest, WorkloadDistributionTest) {
	auto* pool = IoContextPool::getInstance();
	std::vector<std::atomic<int>> context_loads(4);
	const int TOTAL_TASKS = 10000;

	// First get references to all contexts
	std::vector<net::io_context*> contexts;
	for (int i = 0; i < 4; ++i) {
		contexts.push_back(&pool->getIoContext());
	}

	// Record usage count for each context
	for (int i = 0; i < TOTAL_TASKS; ++i) {
		auto& current_context = pool->getIoContext();

		// Find which context we got
		for (size_t j = 0; j < contexts.size(); ++j) {
			if (&current_context == contexts[j]) {
				context_loads[j]++;
				break;
			}
		}
	}

	// Print distribution for debugging
	fmt::println("Task distribution:");
	for (size_t i = 0; i < context_loads.size(); ++i) {
		fmt::println("Context {}: {} tasks", i, context_loads[i].load());
	}

	// Check if load is balanced (allowing 10% margin)
	int expected = TOTAL_TASKS / 4;
	int margin = expected * 0.1;

	for (size_t i = 0; i < context_loads.size(); ++i) {
		EXPECT_NEAR(context_loads[i].load(), expected, margin)
			<< "Context " << i << " has unbalanced load";
	}
}

// 测试异常情况处理
TEST_F(IoContextPoolTest, ExceptionTest) {
	auto* pool = IoContextPool::getInstance();

	// 测试重复启动
	EXPECT_NO_THROW(pool->start(2));

	// 测试停止后再启动
	pool->stop();
	EXPECT_NO_THROW(pool->start(2));
}

// 测试异步操作链
TEST_F(IoContextPoolTest, AsyncChainTest) {
	auto* pool = IoContextPool::getInstance();
	std::atomic<int> stage{0};

	net::post(pool->getIoContext(), [&]() {
		stage = 1;
		net::post(pool->getIoContext(), [&]() {
			stage = 2;
			net::post(pool->getIoContext(), [&]() { stage = 3; });
		});
	});

	// 等待链式操作完成
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_EQ(stage, 3);
}

// 测试并发任务执行
TEST_F(IoContextPoolTest, ConcurrentTasksTest) {
	auto* pool = IoContextPool::getInstance();
	std::atomic<int> counter{0};
	// int counter = 0;
	const int TASK_COUNT = 5000;

	// 提交多个异步任务
	for (int i = 0; i < TASK_COUNT; ++i) {
		net::post(pool->getIoContext(), [&counter]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			counter++;
		});
	}

	// 等待任务完成
	std::this_thread::sleep_for(std::chrono::seconds(5));
	EXPECT_EQ(counter, TASK_COUNT);
}
