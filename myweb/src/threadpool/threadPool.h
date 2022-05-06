#pragma once 
#include<thread>
#include<functional>
#include<queue>
#include <mutex>
#include <condition_variable>
class ThreadPool{
	public:
		explicit ThreadPool(size_t threadCount = 4): pool_(std::make_shared<Pool>()){
			for(size_t i = 0; i < threadCount; i++){
				std::thread([pool = pool_]{
					
					while(true){
						std::unique_lock<std::mutex> locker(pool->mtx);
						if(!pool->tasks.empty()){
							auto task = pool->tasks.front();
							pool->tasks.pop();
							locker.unlock();
							task();
							locker.lock();
						}else if(pool->isClosed){
							break;
						}else{
							pool->cv.wait(locker);
						}
					}
				}).detach();
			}
		}
		ThreadPool() = default;
		ThreadPool(ThreadPool&&) = default;

		~ThreadPool(){
			if(static_cast<bool>(pool_)) {
            	{
                	std::lock_guard<std::mutex> locker(pool_->mtx);
                	pool_->isClosed = true;
            	}
            	pool_->cv.notify_all();
       		}
		}
		template<typename F>
		void AddTask(F&& task){
			{
				std::lock_guard<std::mutex> locker(pool_->mtx);
				pool_->tasks.emplace(std::forward<F>(task));
			}
			pool_->cv.notify_one();
		}
		
	private:
		struct Pool{
			std::mutex mtx;
			std::queue<std::function<void()>> tasks;
			bool isClosed;
			std::condition_variable cv;	
		};
		std::shared_ptr<Pool> pool_;
		
};
