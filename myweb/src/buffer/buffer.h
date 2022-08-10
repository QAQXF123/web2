#pragma once

#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>

class Buffer {
public:
    Buffer(int initBuffSize = 4096);
    ~Buffer() = default;

    size_t WriteableBytes() const;       
    size_t ReadableBytes() const ;
    size_t FreeBytes() const;

    const char* Peek() const;
    void EnsureWriteable(size_t len);
    void HasWritten(size_t len);

    void Get(size_t len);
    void GetUntil(const char* end);

    void clear();
    std::string GetAllToStr();

    const char* BeginWriteConst() const;
    char* BeginWrite();

    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);

    ssize_t ReadFd(int fd, int* Errno);
    ssize_t WriteFd(int fd, int* Errno);

private:
    char* BeginPtr_();
    const char* BeginPtr_() const;
    void MakeSize_(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readIndex_;
    std::atomic<std::size_t> writeIndex_;
};
