#include"buffer.h"
using namespace std;
int main(){
    Buffer buffer;
    char t[] = "aaaaaaaaaaaaaa";
    string s(50000, 'a');
    //buffer.Append(s);
    std::cout<<buffer.WriteableBytes()<<"\n";
    cout<<s.size()<<endl;
    cout<<snprintf(buffer.BeginWrite(), strlen(t)+1, "%s", t);
    
    //int t = buffer.size();
    

}
