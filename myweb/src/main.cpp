#include "server/server.h"
int main() {
   
    Server server(
        12345, 3, 5000, false,             
        3306, "root", "root", "webserver", 
        12, 6, true, 1, 1024, false, false);       
    server.Start();
} 
