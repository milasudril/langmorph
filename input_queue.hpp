#ifndef WORDGEN_INPUTQUEUE_HPP
#define WORDGEN_INPUTQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

namespace wordgen
{
	template<class T>
	class input_queue
	{
	public:
		T front()
		{
			std::lock_guard lock{m_mtx};
			auto ret = std::move(m_fifo.front());
			m_fifo.pop();
			return ret;
		}

		void pop(){}

		bool empty() const
		{
			std::unique_lock lock{m_mtx};
			m_cv.wait(lock, [this](){
				return !m_fifo.empty() || m_stop;
			});
			return m_stop;
		}

		void terminate()
		{
			m_stop = true;
		}

		void push(T&& obj)
		{
			std::lock_guard lock{m_mtx};
			m_fifo.push(std::move(obj));
			m_cv.notify_one();
		}

	private:
		mutable std::mutex m_mtx;
		mutable std::condition_variable m_cv;
		std::queue<T> m_fifo;
		std::atomic<bool> m_stop;
	};
}

#endif