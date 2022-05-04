#include"log.cpp"
#include"../buffer/buffer.h"
int main(){
	auto log = Log::Instance();
    log->init();
    for(int i = 0; i < 1; i++){
        log->write(1, "bbbb\n", i);
    }
   

}
