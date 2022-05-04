#include"httpconn.h"
const char* HttpConnection::srcDir;
std::atomic<int> HttpConnection::userCount;
bool HttpConnection::isET;