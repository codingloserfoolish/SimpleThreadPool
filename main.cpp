#include<iostream>
#include"ThreadPoolCultivate.hpp"
int main()
{
	ThreadPoolEx::instance()->start(4);
	std::mutex log_mtx;

	ThreadPoolEx::instance()->push([&]() {
		log_mtx.lock();
		std::cout << std::this_thread::get_id()<<"正在工作...\n";
		log_mtx.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(5));
		});
	ThreadPoolEx::instance()->push([&]() { 
		log_mtx.lock();
		std::cout << std::this_thread::get_id() << "正在工作...\n";
		log_mtx.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(5)); 
		});
	ThreadPoolEx::instance()->push([&]() {
		log_mtx.lock();
		std::cout << std::this_thread::get_id() << "正在工作...\n";
		log_mtx.unlock();
		std::this_thread::sleep_for(std::chrono::seconds(5));
		});


	std::this_thread::sleep_for(std::chrono::seconds(3));
	return 0;
}