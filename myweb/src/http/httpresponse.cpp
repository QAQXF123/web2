#include"httpresponse.h"

using namespace std;
const unordered_map<string, string> HttpResponse::TYPE = {
    { ".html",  "text/html" },
    { ".xml",   "text/xml" },
    { ".xhtml", "application/xhtml+xml" },
    { ".txt",   "text/plain" },
    { ".rtf",   "application/rtf" },
    { ".pdf",   "application/pdf" },
    { ".word",  "application/nsword" },
    { ".png",   "image/png" },
    { ".gif",   "image/gif" },
    { ".jpg",   "image/jpeg" },
    { ".jpeg",  "image/jpeg" },
    { ".au",    "audio/basic" },
    { ".mpeg",  "video/mpeg" },
    { ".mpg",   "video/mpeg" },
    { ".avi",   "video/x-msvideo" },
    { ".gz",    "application/x-gzip" },
    { ".tar",   "application/x-tar" },
    { ".css",   "text/css "},
    { ".js",    "text/javascript "},
};
const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    { 200, "OK" },
    { 400, "Bad Request" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
};
const unordered_map<int, string> HttpResponse::CODE_PATH = {
    { 400, "/400.html" },
    { 403, "/403.html" },
    { 404, "/404.html" },
};
HttpResponse::HttpResponse(){
    code_ = -1;
    path_ = "";
    isKeepAlive_ = false;
    mmFile_ = nullptr;
    mmFileStat_ = {0};
}
HttpResponse::~HttpResponse(){
    UnmapFile();
}

void HttpResponse::Init(const string& srcDir,const string& path, bool isKeepAlive, int code){
    if(mmFile_){
        UnmapFile();
    }
    code_ = code;
    isKeepAlive_ = isKeepAlive;
    path_ = srcDir + path;
    mmFile_ = nullptr;
    mmFileStat_ = {0};
   // cache_ = new Cache();
}

void HttpResponse::MakeResponse(Buffer &buff){
    if(stat(path_.data(), &mmFileStat_) < 0 || S_ISDIR(mmFileStat_.st_mode)){
        code_ = 404;
    }else if(!(mmFileStat_.st_mode & S_IROTH)){
        code_ = 403;
    }else{
        code_ = 200;
    }
  //  printf("code:%d,path:%s\n", code_, path_.data());
    
    AddStateLine_(buff);
    AddHeader_(buff);
    AddContent_(buff);
}

char* HttpResponse::File(){
    return mmFile_;
}

size_t HttpResponse::FileLen(){
    return mmFileStat_.st_size;
}

void HttpResponse::ErrorHtml_() {
    return;
    if(CODE_PATH.count(code_) == 1) {
        path_ = CODE_PATH.find(code_)->second;
        stat(path_.data(), &mmFileStat_);
    }
}
void HttpResponse::AddStateLine_(Buffer &buff){
    string status;
 /*   if(CODE_STATUS.count(code_)){
        status = CODE_STATUS.find(code_)->second;
    }*/
    status = "200";
    std::string t = "HTTP/1.1 " + to_string(code_) + " " + status + "\r\n";
    buff.Append(t);
}

void HttpResponse::AddHeader_(Buffer &buff){
    buff.Append("Connetion: ");
    buff.Append("close\r\n");
   /* if(isKeepAlive_){
        buff.Append("Keep-Alive\r\n");
    }else{
        buff.Append("close\r\n");
    }*/
    buff.Append("Content-type: " + GetFileType_() + "\r\n");
}

void HttpResponse::AddContent_(Buffer &buff){
   /* printf("addcontent path:%s\n", path_.c_str());
    Cache::mmFileNode *cacheNode = cache_->GetmmFileNode(path_, mmFileStat_.st_size);
    if(cacheNode == nullptr){
        printf("nullptr\n");
        return;
    }
    printf("length:%d\n", cacheNode->length);
    if(cacheNode->mmFile == NULL){
        printf("null????\n");
    }else{
        printf("mmfile:%s\n", mmFile_);
    }
    mmFile_ = cacheNode->mmFile;
    
    printf("mmfile:%s\n", mmFile_);*/
   /* int srcFd = open(path_.data(), O_RDONLY);
  //  printf("srcFd:%d\n", srcFd);
    if(srcFd < 0){
        return;
    }
    int* mmRet = (int*)mmap(0, mmFileStat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if(*mmRet == -1){
        return; 
    }
    mmFile_ = (char*)mmRet;
    close(srcFd);*/
    buff.Append("Content-Length: " + to_string(mmFileStat_.st_size) + "\r\n\r\n");
  //  printf("content-len:%d\n", mmFileStat_.st_size);
}
void HttpResponse::UnmapFile(){
    if(mmFile_){
        munmap(mmFile_, mmFileStat_.st_size);
        mmFile_ = nullptr;
    }
}

string HttpResponse::GetFileType_(){
    string::size_type idx = path_.find_last_of('.');
    if(idx == string::npos){
        return "text/plain";
    }
    string suffix = path_.substr(idx);
    if(TYPE.count(suffix)){
        return TYPE.find(suffix)->second;
    }
    return "text/plain";
}