#pragma once
#include<condition_variable>
#include<mutex>

namespace bustub{
    using mutex_t = std::mutex;
    using cond_t = std::condition_variable;
    static const uint32_t MAX_READERS = -1;
    class RWlock
    {
    public:
        RWlock() = default;
        ~RWlock(){
            std::lock_guard<mutex_t> guard(mtx_);
        }
        void WLock(){
            std::unique_lock<mutex_t> lock(mtx_);
            while(writerEntered_){
                reader_.wait(lock);
            }
            writerEntered_ = true;
            while(readCount_ > 0){
                writer_.wait(lock);
            }
        }
        void WUnlock(){
            std::lock_guard<mutex_t> guard(mtx_);
            writerEntered_ = false;
            reader_.notify_all();
        }
        void RLock(){
            std::unique_lock<mutex_t> lock(mtx_);
            while(writerEntered_ || readCount_ == MAX_READERS){
                reader_.wait(lock);
            }
            readCount_++;

        }
        void RUnlock(){
            std::unique_lock<mutex_t> lock(mtx_);
            readCount_--;
            if(writerEntered_){
                if(readCount_ == 0){
                    writer_.notify_one();
                }
            }else if(readCount_ == MAX_READERS - 1){
                reader_.notify_one();
            }
        }
    private:
        mutex_t mtx_;
        cond_t writer_;
        cond_t reader_;
        uint32_t readCount_{0};
        bool writerEntered_{false};
    };
    

   
    
}
