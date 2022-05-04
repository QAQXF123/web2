#include"log.h"
#include<iostream>

using namespace std;

Log::Log(){
    lines_ = 0;
    isAsync_ = false;
    writeThread_ = nullptr;
    deque_ = nullptr;
    file_ = nullptr;
}

Log::~Log(){
    if(writeThread_ && writeThread_->joinable()){
        while(!deque_->empty()){
            deque_->flush();
        }
        deque_->Close();
        writeThread_->join();
    }
    if(file_){
        lock_guard<mutex> locker(mtx_);
        flush();
        fclose(file_);
    }
}

int Log::GetLevel(){
    lock_guard<mutex> locker(mtx_);
    return level_;
}

void Log::SetLevel(int level){
    lock_guard<mutex> locker(mtx_);
    level_ = level;
}

void Log::init(int level, const char *path, const char* suffix, int capacity){
    isOpen_ = true;
    level_ = level;
    
    if(capacity > 0){
        isAsync_ = true;
        if(!deque_){
            unique_ptr<BlockDeque<string>> deq(new BlockDeque<string>(1024));
            deque_ = move(deq);
            unique_ptr<thread> thread(new std::thread(FlushLogThread));
            writeThread_ = move(thread);  
        }
    }else{
        isAsync_ = false;
    }
    
    lines_ = 0;


    time_t timer = time(nullptr);  
    struct tm *sysTime = localtime(&timer);
    struct tm t = *sysTime;
    path_ = path;
    suffix_ = suffix;
    char fileName[LOG_NAME_LEN] = {0};
    snprintf(fileName, LOG_NAME_LEN - 1, "%s/%04d_%02d_%02d%s", 
            path_, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, suffix_);
    today_ = t.tm_mday;

    {
        lock_guard<mutex> locker(mtx_);
        buff_.clear();
        if(file_){
            flush();
            fclose(file_);
        }

        file_ = fopen(fileName, "a"); 
        if(file_ == nullptr){
            mkdir(path_, 777);
            file_ = fopen(fileName, "a");
        }
    }
}

void Log::write(int level, const char *format, ...){
	
    
    	
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t tsec = now.tv_sec;
    struct tm *sysTime = localtime(&tsec);
    struct tm t = *sysTime;
    va_list vaList;
    if(today_ != t.tm_mday || lines_ == MAX_LINES){
        
        unique_lock<mutex> locker(mtx_);
        locker.unlock();
        char newFileName[LOG_NAME_LEN];
        char tail[36] = {0};
        snprintf(tail, 36, "%04d_%02d_%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday);

        if(today_  != t.tm_mday){
            snprintf(newFileName, LOG_NAME_LEN - 72, "%s %s %s", path_, tail, suffix_);
            today_ = t.tm_mday;
            lines_ = 0;
        }else{
            snprintf(newFileName, LOG_NAME_LEN -  72, "%s %s %s", path_, tail, suffix_);
        }
        locker.unlock();
        flush();
        fclose(file_);
        file_ = fopen(newFileName, "a");
    }

    {
        unique_lock<mutex> locker(mtx_);
        lines_++;
      //  printf("line%d\n", lines_);
        int n = snprintf(buff_.BeginWrite(), 128, "%d-%02d-%02d %02d:%02d:%02d.%06ld ",
                    t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                    t.tm_hour, t.tm_min, t.tm_sec, now.tv_usec);
        
        buff_.HasWritten(n);

        AppendLogLevelTitle_(level_);

        va_start(vaList, format);
        int m = vsnprintf(buff_.BeginWrite(), buff_.WriteableBytes(), format, vaList);
        va_end(vaList);

        buff_.HasWritten(m);
        buff_.Append("\n\0", 2);

        if(isAsync_ && deque_ && !deque_->full()) {
            deque_->push_back(buff_.GetAllToStr());
        } else {
            fputs(buff_.Peek(), file_);
        }
        buff_.clear();
        
    }
}

void Log::AppendLogLevelTitle_(int level){
    switch(level){
        case 0:
            buff_.Append("[debug]:", 9);
            break;
        case 1:
            buff_.Append("[info]:", 7);
            break;
        case 2:
            buff_.Append("[warn]:", 9);
            break;
        case 3:
            buff_.Append("[error]:", 9);
            break;
        case 4:
            buff_.Append("[info]:", 9);
            break;
    }
}

void Log::flush(){
    if(isAsync_){	
    	deque_->flush();
    }
    fflush(file_);
}

void Log::AsyncWrite_(){
    string str = "";
    while(deque_->pop(str)){
        lock_guard<mutex> locker(mtx_);
        fputs(str.c_str(), file_);
    }
}

Log* Log::Instance(){
    static Log inst;
    return &inst;
}

void Log::FlushLogThread(){
    Log::Instance()->AsyncWrite_();
}



