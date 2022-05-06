#include"server.h"
#include"../util/util.h"
#include<iostream>

using namespace std;

Server::Server(
            int port, int trigMode, int timeOutMS, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, int logLevel, int logQueSize, bool useCache, bool isDebug):
            port_(port), openLinger_(OptLinger), timeOutMS_(timeOutMS), 
            isClose_(false), isDebug_(isDebug), useCache_(useCache),
            threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller()), socket_(new Socket(port)),
            cache_(new Cache()), timer_(new Timer()), tableManager_(new bustub::TableManager())
            
{

        

    srcDir_ = getcwd(nullptr, 256);
    assert(srcDir_);
    strncat(srcDir_, "/resources/", 16);
    HttpConnection::userCount = 0;
    HttpConnection::srcDir = srcDir_;
    HttpRequest::tableManager = tableManager_.get();
    timeOutMS_ = 0;
    InitEventMode_(0);
    listenFd_ = socket_->Init();
    //Dprintf("listenFd:%d, srcDir:%s\n", listenFd_, srcDir_);
    openLog = false;
    if(listenFd_ == -1  || epoller_->AddFd(listenFd_,  listenEvent_ | EPOLLIN) == 0) {
        isClose_ = true;
    }

    if(openLog) {
        
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);
        if(isClose_) { LOG_ERROR("========== Server init error!=========="); }
        else {
            LOG_INFO("========== Server init ==========");
            LOG_INFO("Port:%d, OpenLinger: %s", port_, OptLinger? "true":"false");
            LOG_INFO("Listen Mode: %s, OpenConn Mode: %s",
                            (listenEvent_ & EPOLLET ? "ET": "LT"),
                            (connEvent_ & EPOLLET ? "ET": "LT"));
            LOG_INFO("LogSys level: %d", logLevel);
            LOG_INFO("srcDir: %s", HttpConnection::srcDir);
            LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
        }
        if(isDebug_){
            //数据库测试
            Dprintf("database test beign\n\n\n");
            LOG_INFO("database test1 begin");
            tableManager_->test1();
            LOG_INFO("database test1 end");
            LOG_INFO("database test2 begin");
            tableManager_->test2();
            LOG_INFO("database test2 end");
            LOG_INFO("database test3 begin");
            tableManager_->test3();
            LOG_INFO("database test3 end");
            auto rands = tableManager_->GetRands(1e6);
            LOG_INFO("database test4 begin");
            tableManager_->test4(rands);
            LOG_INFO("database test4 end");
            Dprintf("database test end\n\n\n");
        }
       
    }
}

Server::~Server() {
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
}


void Server::Start() {
    int timeMS = -1;  /* epoll wait timeout == -1 无事件将阻塞 */
    if(!isClose_) { LOG_INFO("========== Server start =========="); }
    while(!isClose_) {
        if(timeOutMS_ > 0) {
            timeMS = timer_->GetNextTick();
        }
        //epoll wait 
        int eventCnt = epoller_->Wait(timeMS);
        for(int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventsFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if(fd == listenFd_) {  
                //处理监听套接字
                DealListen_();
            }else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                //EPOLLRDHUP 连接关闭
                //EPOLLHUP
                //EPOLLERR 连接出错
                assert(users_.count(fd) > 0);
                //其他条件
                CloseConn_(&users_[fd]);
            }else if(events & EPOLLIN) {
                assert(users_.count(fd) > 0);
                //套接字可读
                DealRead_(&users_[fd]);
            }else if(events & EPOLLOUT) {
                assert(users_.count(fd) > 0);
                //套接字可写
                DealWrite_(&users_[fd]);
            } else {
                //位置情况
                LOG_ERROR("Unexpected event");
            }
        }
    }
} 

void Server::InitEventMode_(int trigMode){
    listenEvent_ |= EPOLLRDHUP;
    connEvent_ |= EPOLLRDHUP | EPOLLONESHOT;  
    //EPOLLONESHOT 每次处理时候都关闭事件监听,避免多线程抢占同一套接字
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
    Dprintf("Client[%d] quit!\n", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
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

void Server::AddClient_(int fd, sockaddr_in &addr) {
    assert(fd > 0);
    users_[fd].Init(fd, addr);
    if(timeOutMS_ > 0) {
        timer_->add(fd, timeOutMS_, std::bind(&Server::CloseConn_, this, &users_[fd]));
    }
    SetFdNonblock(fd);
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    
    LOG_INFO("Client[%d] in!", users_[fd].GetFd());
    Dprintf("Client[%d] in\n", users_[fd].GetFd());
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
        }
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
    }
       
}

void Server::OnWrite_(HttpConnection *client) {
    assert(client);
    int writeErrno = 0;
    client->write(&writeErrno);
    if(client->MoreBytes() == 0){
        CloseConn_(client);
    }else{
       epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);     
    }
}


int Server::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}