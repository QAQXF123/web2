#pragma once
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<assert.h>
#include<vector>
#include<errno.h>

class Epoller{
    public:
        explicit Epoller(int maxEvent = 4096);
        ~Epoller();

        bool AddFd(int fd, uint32_t events);
        bool ModFd(int fd, uint32_t events);
        bool DelFd(int fd);
        int Wait(int timeOutMs = -1);
        int GetEventsFd(size_t i) const;
        uint32_t GetEvents(size_t i) const;


    private:
        int epollFd_;
        std::vector<struct epoll_event> events_;
};