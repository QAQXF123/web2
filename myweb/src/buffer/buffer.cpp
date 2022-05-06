#include "buffer.h"

Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readIndex_(0), writeIndex_(0) {}

size_t Buffer::ReadableBytes() const {
    return writeIndex_ - readIndex_;
}
size_t Buffer::WriteableBytes() const {
    return buffer_.size() - writeIndex_;
}

size_t Buffer::FreeBytes() const {
    return readIndex_;
}

const char* Buffer::Peek() const {
    return BeginPtr_() + readIndex_;
}

void Buffer::Get(size_t len) {
    assert(len <= ReadableBytes());
    readIndex_ += len;
}

void Buffer::GetUntil(const char* end) {
    assert(Peek() <= end );
    Get(end - Peek());
}

void Buffer::clear() {
   
    readIndex_ = 0;
    writeIndex_ = 0;
}

std::string Buffer::GetAllToStr() {
    std::string str(Peek(), ReadableBytes());
    clear();
    return str;
}

const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writeIndex_;
}

char* Buffer::BeginWrite() {
    return BeginPtr_() + writeIndex_;
}

void Buffer::HasWritten(size_t len) {
    writeIndex_ += len;
} 

void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}



void Buffer::EnsureWriteable(size_t len) {
    if(WriteableBytes() < len) {
        MakeSize_(len);
    }
    assert(WriteableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    char buff[4096];
    struct iovec iov[2];
    const size_t writable = WriteableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writeIndex_;
    iov[0].iov_len = writable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if(len < 0) {
        *saveErrno = errno;
    } else if(static_cast<size_t>(len) <= writable) {
        writeIndex_ += len;
    } else {
        writeIndex_ = buffer_.size();
        Append(buff, len - writable);
    }
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readIndex_ += len;
    return len;
}

char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

void Buffer::MakeSize_(size_t len) {
    if(WriteableBytes() + FreeBytes() < len) {
        buffer_.resize(writeIndex_ + len + 1);
    } else {
        size_t readable = ReadableBytes();
        std::copy(BeginPtr_() + readIndex_, BeginPtr_() + writeIndex_, BeginPtr_());
        readIndex_ = 0;
        writeIndex_ =  readable;
        assert(readable == ReadableBytes());
    }
}