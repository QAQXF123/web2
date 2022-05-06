#pragma once

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>   
#include "httprequest.h"
#include "httpresponse.h"
#include <mutex>
class HttpConnection{
    public:
        HttpConnection(){
            fd_ = 0;
            addr_ = {0};
            isClose_ = true;
        }

        ~HttpConnection(){
            Close();
        }

        void Init(int fd, const sockaddr_in& addr){
            assert(fd > 0);
            userCount++;
            addr_ = addr;
            fd_ = fd;
            writeBuff_.clear();
            readBuff_.clear();
            isClose_ = false;
        }

        void Close(){
            rsp_.UnmapFile();
            if(isClose_ == false){
                isClose_ = true;
                userCount--;
                close(fd_);
            }
        }
        
        long GetFileLen() const{
            return iov_[1].iov_len;
        }
        int GetFd() const{
            return fd_;
        }
        struct sockaddr_in GetSockAddr() const{
            return addr_;
        }
        const char* GetIp() const{
            return inet_ntoa(addr_.sin_addr);
        }
        int GetPort() const{
            return addr_.sin_port;
        }
        ssize_t read(int *saveErrno){
            ssize_t len = -1;
            do{
                len = readBuff_.ReadFd(fd_, saveErrno);
                if(len <= 0){
                    break;
                }

            } while(isET);
            return len;    
        }

        bool MoreBytes(){
            return iov_[0].iov_len > 0 ||  iov_[1].iov_len > 0;
        }

        ssize_t write(int *saveErrno){
            ssize_t len = -1;
            do{
                len = writev(fd_, iov_, iovCnt_);
                if(len <= 0){
                    *saveErrno = errno;
                    break;
                }
                if(!MoreBytes()){
                    break;
                }else if(static_cast<size_t>(len) > iov_[0].iov_len){
                    iov_[1].iov_base = (uint8_t*) iov_[1].iov_base + (len - iov_[0].iov_len);
                    iov_[1].iov_len -= (len - iov_[0].iov_len);
                    if(iov_[0].iov_len){
                        iov_[0].iov_len = 0;
                        writeBuff_.clear();
                    }
                }else{
                    iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len; 
                    iov_[0].iov_len -= len; 
                    writeBuff_.Get(len);
                }

            } while(isET);
            return len;
        }

        std::string process(){
            req_.Init();
            if(readBuff_.ReadableBytes() <= 0){
                return "fail";
            }else if(req_.parse(readBuff_)){ 
                rsp_.Init(srcDir, req_.path(), false, 200);
            }else{
                //printf("parse fail\n");
                return "fail";
            }
                
           //Dprintf("srcDir:%s, path:%s\n", srcDir, req_.path().c_str());
            rsp_.MakeResponse(writeBuff_);
            //Dprintf("make rsp_\n");
            iov_[0].iov_base = const_cast<char*>(writeBuff_.Peek());
            iov_[0].iov_len = writeBuff_.ReadableBytes();
            iovCnt_ = 1;
            if(rsp_.FileLen() > 0 /* && rsp_.File()*/) {
                iov_[1].iov_len = rsp_.FileLen();
                iovCnt_ = 2;
                return rsp_.path();
               //*iov_[1].iov_base = rsp_.File();*/    
            }
            return "";
        }

        void setFile(char *mmFile){
            iov_[1].iov_base = mmFile;

        }
        static const char* srcDir;
        static std::atomic<int> userCount;
        static bool isET;
    private:
        int fd_;
        struct sockaddr_in addr_;
        bool isClose_;
        int iovCnt_;
        struct iovec iov_[2];
        Buffer readBuff_;
        Buffer writeBuff_;
        HttpRequest req_;
        HttpResponse rsp_;
     
};
