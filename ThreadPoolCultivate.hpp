#ifndef THREADPOOLCULTIVATE_H_
#define THREADPOOLCULTIVATE_H_

#include<thread>
#include<atomic>
#include<queue>
#include<future>
#include<condition_variable>

#include<iostream>

#include"SingleInstance.hpp"

class ThreadPoolEx :public SingleInstance<ThreadPoolEx>
{
	friend class SingleInstance<ThreadPoolEx>;
public:
	virtual ~ThreadPoolEx()
	{
		m_stop.store(true);
		m_cv.notify_all();
		for (std::thread& th : m_threads)
		{
			m_log_mtx.lock();
			std::cout << th.get_id() << "退出了" << std::endl;
			m_log_mtx.unlock();
			if(th.joinable())th.join();
		}
	}
	template<class Func,class ...Args>
	void push(Func&& task, Args&&...args)
	{
		if (m_stop.load())
		{
			throw std::runtime_error("ThreadPool has Stopped!!!");
		}
		std::lock_guard<std::mutex> lck(m_mtx);
		m_tasks.emplace(std::bind([task](Args&&...args) { task(args...); }, std::forward<Args>(args)...));
		m_cv.notify_one();
	}

	void start(int thread_count = 2)
	{
		int max_thread_count = std::thread::hardware_concurrency();
		if (thread_count < max_thread_count)
		{
			max_thread_count = thread_count;
		}
		m_stop.store(false);
		m_total_threads.store(max_thread_count);
		m_relaxing_threads.store(max_thread_count);

		for (int i = 0; i < max_thread_count; ++i)
		{
			m_threads.emplace_back(std::thread([this]() {
				Task task([]() {});
				m_log_mtx.lock();
				std::cout << std::this_thread::get_id() << "启动了\n" << std::endl;
				m_log_mtx.unlock();
				while (!m_stop.load())
				{

					{
						std::unique_lock<std::mutex> lck(m_mtx);
						m_cv.wait(lck, [&]() {return m_stop.load() || !m_tasks.empty(); });
						if (!m_tasks.empty())
						{
							task = std::move(m_tasks.front());
							m_tasks.pop();
						}
					}
					--m_relaxing_threads;
					task();
					++m_relaxing_threads;
				}
				}));
		}
	}



private:

	std::atomic_int										m_relaxing_threads;
	std::atomic_int										m_total_threads;
	std::vector<std::thread>							m_threads;
	std::queue<std::packaged_task<void()>>				m_tasks;
	std::atomic_bool									m_stop;
	std::mutex											m_mtx;
	std::mutex											m_log_mtx;
	std::condition_variable								m_cv;


	using Task = std::packaged_task<void()>;
};

#endif // !THREADPOOLCULTIVATE_H_
