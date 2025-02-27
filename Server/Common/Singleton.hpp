#pragma once
#include <memory>
#include <mutex>

template <class T>
class Singleton {
public:
	template <typename... Args>
	static T* getInstance(Args&&... args) {
		static std::once_flag flag;

		std::call_once(flag, [&]() { m_ptr.reset(new T(std::forward<Args>(args)...)); });

		return m_ptr.get();
	}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

private:
	static std::unique_ptr<T> m_ptr;
};

template <class T>
std::unique_ptr<T> Singleton<T>::m_ptr = nullptr;