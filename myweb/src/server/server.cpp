#include"server.h"
#include<iostream>

using namespace std;

Server::Server(
            int port, int trigMode, int timeOutMS, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, int logLevel, int logQueSize):
            port_(port), openLinger_(OptLinger), timeOutMS_(timeOutMS), isClose_(false),
            threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller()), socket_(new Socket(port)),
            cache_(new Cache()), timer_(new Timer()), tableManager_(new bustub::TableManager())
            
    {

        

        printf("database test1 end\n\n\n\n");
    srcDir_ = getcwd(nullptr, 256);
    
    assert(srcDir_);
    strncat(srcDir_, "/resources/", 16);
    HttpConnection::userCount = 0;
    HttpConnection::srcDir = srcDir_;
    HttpRequest::tableManager = tableManager_.get();
   // printf("srcDir:%s\n", srcDir_);

    
    timeOutMS_ = 0;

    InitEventMode_(0);
    listenFd_ = socket_->Init();
   // printf("listenFd:%d, srcDir:%s\n", listenFd_, srcDir_);
    openLog = true;
    if(listenFd_ == -1  || epoller_->AddFd(listenFd_,  listenEvent_ | EPOLLIN) == 0) {
        isClose_ = true;
    }

    if(openLog) {
        
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);
        if(isClose_) { LOG_ERROR("========== Server init error!=========="); }
        else {
            //printf("init log\n");
            LOG_INFO("========== Server init ==========");
            LOG_INFO("Port:%d, OpenLinger: %s", port_, OptLinger? "true":"false");
            LOG_INFO("Listen Mode: %s, OpenConn Mode: %s",
                            (listenEvent_ & EPOLLET ? "ET": "LT"),
                            (connEvent_ & EPOLLET ? "ET": "LT"));
            LOG_INFO("LogSys level: %d", logLevel);
            LOG_INFO("srcDir: %s", HttpConnection::srcDir);
            LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
        }
       
        printf("database test beign\n\n\n");

        LOG_INFO("database test1 begin");
        tableManager_->test1();
        LOG_INFO("database test1 end");

        LOG_INFO("database test2 begin");
        tableManager_->test2();
        LOG_INFO("database test2 end");

       /* LOG_INFO("database test3 begin");
        tableManager_->test3();
        LOG_INFO("database test3 end");*/

        auto rands = tableManager_->GetRands(1e6);

        /*LOG_INFO("database test4 begin");
        
        tableManager_->test4(rands);
        LOG_INFO("database test4 end");*/

        printf("database test end\n\n\n");
    }
}

Server::~Server() {
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
   // SqlConnPoolInstance()->ClosePool();
}


void Server::Start() {
    int timeMS = -1;  /* epoll wait timeout == -1 无事件将阻塞 */
    if(!isClose_) { LOG_INFO("========== Server start =========="); }
    while(!isClose_) {
        if(timeOutMS_ > 0) {
            timeMS = timer_->GetNextTick();
        }
        int eventCnt = epoller_->Wait(timeMS);
    //    printf("eventCnt:%d\n", eventCnt);
        for(int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventsFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if(fd == listenFd_) {
                DealListen_();
            }else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            }else if(events & EPOLLIN) {
             //   printf("in\n");
                assert(users_.count(fd) > 0);
                DealRead_(&users_[fd]);
            }else if(events & EPOLLOUT) {
             //   printf("out\n");
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]);
            } else {
                LOG_ERROR("Unexpected event");
            }
        }
    }
} 

void Server::InitEventMode_(int trigMode){
    listenEvent_ |= EPOLLRDHUP;
    connEvent_ |= EPOLLRDHUP | EPOLLONESHOT;
    if(trigMode & 1){
        listenEvent_ |= EPOLLET;        
    }
    if(trigMode & 2){
        connEvent_ |= EPOLLET;
        HttpConnection::isET = true;
    }
}



void Server::CloseConn_(HttpConnection* client) {
    assert(client);
    LOG_INFO("Client[%d] quit!", client->GetFd());
    printf("Client[%d] quit!\n", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
}

void Server::AddClient_(int fd, sockaddr_in &addr) {
    assert(fd > 0);
    users_[fd].Init(fd, addr);
    if(timeOutMS_ > 0) {
        timer_->add(fd, timeOutMS_, std::bind(&Server::CloseConn_, this, &users_[fd]));
    }
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    SetFdNonblock(fd);
    LOG_INFO("Client[%d] in!", users_[fd].GetFd());
    printf("Client[%d] in\n", users_[fd].GetFd());
}

void Server::DealListen_() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    do {
        int fd = accept(listenFd_, (struct sockaddr*)&addr, &len);
        if(fd <= 0) {
             return;
        }else if(HttpConnection::userCount == MAX_FD) {
            LOG_WARN("Clients is full!");
            return;
        }
        AddClient_(fd, addr);
    } while(listenEvent_ & EPOLLET);
}

void Server::DealRead_(HttpConnection *client) {
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&Server::OnRead_, this, client));
}
    

void Server::DealWrite_(HttpConnection *client) {

    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&Server::OnWrite_, this, client));   
}

void Server::ExtentTime_(HttpConnection *client) {
    assert(client);
    if(timeOutMS_ > 0) { timer_->adjust(client->GetFd(), timeOutMS_); }
}

void Server::OnRead_(HttpConnection *client) {
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);
   
    if(ret <= 0){
        return;
    }
    
    OnProcess(client);
}

void Server::OnProcess(HttpConnection *client) {
    string statu = client->process();
    if(statu == "fail") {
         epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLIN);
    } else {
        if(!statu.empty()){
            Cache::mmFileNode *cacheNode = cache_->GetmmFileNode(statu, client->GetFileLen());
            client->setFile(cacheNode->mmFile);
          //  printf("filelen:%ld\n", client->GetFileLen());
        }
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
    }
       
}

void Server::OnWrite_(HttpConnection *client) {
    assert(client);
    int writeErrno = 0;
    client->write(&writeErrno);
    if(client->MoreBytes() == 0){
     //   printf("no more bytes\n");
        CloseConn_(client);
       /* if(client->IsKeepAlive()){
            OnProcess(client);    
        }*/
    }else{
       epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);     
    }
   

}


int Server::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}