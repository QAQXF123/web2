#pragma once

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>


#include "../epoll/epoll.h"
#include "../log/log.h"
#include "../threadpool/threadPool.h"
#include "../http/httpconn.h"
#include "../socket/socket.h"
#include "../cache/cache.h"
#include "../timer/timer.h"
#include "../database/main/table_manager.h"


class Server{
    public:
        Server(int port, int trigMode, int timeOutMS, bool OptLinger,
        int sqlPort, const char *sqlUser, const char *sqlPwd, const char *dbName, int connPoolNum,
        int threadNum,
        bool openLog, int logLevel, int logQueSize);

        ~Server();
        void Start();
    private:
        bool InitSocket_(); 
        void InitEventMode_(int trigMode);
        void AddClient_(int fd, sockaddr_in &addr);
  
        void DealListen_();
        void DealWrite_(HttpConnection* client);
        void DealRead_(HttpConnection* client);

        void SendError_(int fd, const char*info);
        void ExtentTime_(HttpConnection* client);
        void CloseConn_(HttpConnection* client);

        void OnRead_(HttpConnection* client);
        void OnWrite_(HttpConnection* client);
        void OnProcess(HttpConnection* client);

        static const int MAX_FD = 65536;

        static int SetFdNonblock(int fd);

        int port_;
        bool openLinger_;
        int timeOutMS_;  /* 毫秒MS */
        bool isClose_;
        int listenFd_;
        char* srcDir_;
    
        uint32_t listenEvent_;
        uint32_t connEvent_;

        std::unique_ptr<ThreadPool> threadpool_;
        std::unique_ptr<Epoller> epoller_;
        std::unordered_map<int, HttpConnection> users_;
        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Cache> cache_;
        std::unique_ptr<Timer> timer_;
        std::unique_ptr<bustub::TableManager> tableManager_;
};