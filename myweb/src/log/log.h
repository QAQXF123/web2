#pragma once
#include <string>
#include <mutex>
#include <string>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>           // vastart va_end
#include <assert.h>
#include <sys/stat.h>         //mkdir
#include "blockdeque.h"
#include "../buffer/buffer.h"
class Log {
    public:
        void init(int level = 1, const char *path = "./log", 
        const char *suffix = ".log", int capacity = 1024);

        static Log* Instance();
        static void FlushLogThread();

        void write(int level, const char *format, ...);
        void flush();
        
        int GetLevel();
        void SetLevel(int level);
        bool IsOpen(){
            return isOpen_;
        }


    private:
        Log();
        ~Log();
        void AppendLogLevelTitle_(int level);
        void AsyncWrite_();

    private:    
        static const int LOG_PATH_LEN = 256;
        static const int LOG_NAME_LEN = 256;
        static const int MAX_LINES = 64 * 1024;
        const char* path_;
        const char* suffix_;

        int lines_;
        int today_;
        bool isOpen_;
        bool isAsync_;

        Buffer buff_;
        int level_;
        
        
        FILE* file_;
        std::unique_ptr<BlockDeque<std::string>> deque_;
        std::unique_ptr<std::thread> writeThread_;
        std::mutex mtx_;    
};

#define LOG_BASE(level, format, ...) \
    do {\
        Log* log = Log::Instance();\
        if (log->IsOpen() && log->GetLevel() <= level) {\
            log->write(level, format, ##__VA_ARGS__); \
            log->flush();\
        }\
    } while(0);

#define LOG_DEBUG(format, ...) do {LOG_BASE(0, format, ##__VA_ARGS__)} while(0);
#define LOG_INFO(format, ...) do {LOG_BASE(1, format, ##__VA_ARGS__)} while(0);
#define LOG_WARN(format, ...) do {LOG_BASE(2, format, ##__VA_ARGS__)} while(0);
#define LOG_ERROR(format, ...) do {LOG_BASE(3, format, ##__VA_ARGS__)} while(0);
