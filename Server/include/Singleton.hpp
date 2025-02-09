#pragma once
#include <memory>
#include <mutex>

template <class T>
class Singleton {
public:
	static T* getInstance() {
		static std::once_flag flag;
		std::call_once(flag, []() { m_ptr.reset(new T{}); });
		return m_ptr.get();
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

private:
	static std::unique_ptr<T> m_ptr;
};

template <class T>
std::unique_ptr<T> Singleton<T>::m_ptr = nullptr;