#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <exception>

namespace BaseLib
{
	template <class T>
	class ThreadLoop
	{
	public:
		ThreadLoop(std::function<void(T*)> func, T* _this, std::chrono::seconds period)
		{
			Function = func;
			p_this = _this;
			Period = period;
		}

		ThreadLoop() = default;

		void Start()
		{
			if (p_IsRunning)
				throw std::exception("Already running!");

			Thread = std::thread(&ThreadLoop::Loop, this);
			Thread.detach();
		}
		void Stop()
		{
			p_IsRunning = false;
		}

		bool IsRunning()
		{
			return p_IsRunning;
		}
		bool IsEmpty()
		{
			return (Function == nullptr);
		}

		ThreadLoop(const ThreadLoop& obj)
		{

		}
		ThreadLoop(ThreadLoop&& obj) noexcept
		{

		}
		ThreadLoop& operator=(const ThreadLoop& obj)
		{
			ThreadLoop temp(obj);

			temp.swap(*this);

			return *this;
		}
		ThreadLoop& operator=(ThreadLoop&& obj) noexcept
		{
			ThreadLoop temp(std::move(obj));

			temp.swap(*this);

			return *this;
		}

	private:
		std::atomic<bool> p_IsRunning = false;
		std::function<void(T*)> Function;
		T* p_this = nullptr;
		std::chrono::seconds Period;

		std::thread Thread;

		void Loop()
		{
			while (p_IsRunning)
			{
				Function(p_this);
				std::this_thread::sleep_for(Period);
			}
		}

		void swap(ThreadLoop& obj) noexcept
		{
			p_IsRunning = obj.p_IsRunning.exchange(p_IsRunning);
			std::swap(Function, obj.Function);
			std::swap(p_this, obj.p_this);
			std::swap(Period, obj.Period);
			std::swap(Thread, obj.Thread);
		}
	};
}