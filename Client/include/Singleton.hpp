#pragma once

#include <memory>
#include <mutex>

// 单例类模板
template <class T>
class Singleton {
public:
	static T* getInstance() {
		static std::once_flag flag;
		// 调用 T 的构造函数
		std::call_once(flag, []() { instance.reset(new T()); });
		return instance.get();
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

private:
	static std::unique_ptr<T> instance;
};

template <class T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;