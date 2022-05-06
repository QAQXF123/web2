#include"socket.h"
const int MIN_PORT_NUM = 1024;
const int MAX_PORT_NUM = 65536;
int Socket::Init(){
    if(port_ > MAX_PORT_NUM || port_ <= MIN_PORT_NUM){ 
    	LOG_ERROR("PORT ERROR");
        return false;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr = htonl(INADDR_ANY); //use ip with localhost   INADDR_ANY = 0
    addr.sin_addr.s_addr = INADDR_ANY;  
    addr.sin_port = htons(port_);
    int ret;
    bool success = false;
    //inet_addr s->int, inet_ntoa  int->s
    
   
    /*
    struct linger optLinger = {0, 0};
    if(openLiger_){
        optlinger.l_onoff = 1;
        optlinger.l_linger = 1;
    }
    */
    do{ 
        //socket(domain, type, 0)
        //stream tcp, dgram udp
        listenFd_ = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, 0);
        
        if(listenFd_ < 0){
            LOG_ERROR("SOCK INIT ERROR");
            break;
        }

       /* optLinger.l_onoff = 1;
        optLinger.l_linger = 1;*/

        /*ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
        if(ret < 0){
            LOG_ERROR("SET SOCK LINGER ERROR");
            close(listenFd_);
            break;
        }*/

        //set reuseaddr  
        int optval = 1;
        ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
        if(ret == -1) {
            LOG_ERROR("SET SOCK REUSE ERROR");
            close(listenFd_);
            break;
        }
        
        ret = bind(listenFd_, (struct sockaddr*)&addr, sizeof(addr));
        //bind sock with port

        if(ret < 0){
            close(listenFd_);
            LOG_ERROR("SOCK BIND ERROR");
            close(listenFd_);
            break;
        }

        ret = listen(listenFd_, SOMAXCONN);

        if(ret < 0){
            close(listenFd_);
            LOG_ERROR("SOCK LISTEN ERROR");
            break;
        }

        success = true;

    } while(0);

    if(success){
        return listenFd_;
    }else{
        return -1;
    }
}


/* 
    clinet use sock -> sockfd
    connect(sockfd, sturct sockaddr *serv_addr, int addrlen)




*/