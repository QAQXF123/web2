#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include"../log/log.h"

class Socket{
    
    public:
        Socket(int port) :port_(port){};
        ~Socket() = default;
        int Init();
    private:
        int port_;
        int listenFd_;
};