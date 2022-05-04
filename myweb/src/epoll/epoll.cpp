#include"epoll.h"

Epoller::Epoller(int maxEvent):epollFd_(epoll_create(1)), events_(maxEvent){

} 

Epoller::~Epoller(){
    close(epollFd_);
}

bool Epoller::AddFd(int fd, uint32_t events){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) == 0;
}

bool Epoller::ModFd(int fd, uint32_t events){
    if(fd < 0) return false;
    epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = events;
    return epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) == 0;
}

bool Epoller::DelFd(int fd){
    if(fd < 0) return false;
    epoll_event ev = {0};
    return epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev) == 0;
}

int Epoller::Wait(int TimeOutMs){
    return epoll_wait(epollFd_, &events_[0], static_cast<int>(events_.size()), TimeOutMs);
}

int Epoller::GetEventsFd(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
    assert(i < events_.size() && i >= 0);
    return events_[i].events;
}