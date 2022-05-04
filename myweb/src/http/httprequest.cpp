#include"httprequest.h"

using namespace std;
bustub::TableManager* HttpRequest::tableManager;
const unordered_set<string> HttpRequest::HTML_{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const unordered_map<string, int> HttpRequest::HTML_TAG_ {
            {"/register.html", 0}, {"/login.html", 1},  };


void HttpRequest::Init(){
    method_ = path_ = version_ = body_ = "";
    state_ = PARSE_STATE::REQUEST_LINE;
    header_.clear();
    post_.clear();

}

bool HttpRequest::IsKeepAlive() const {
	//return false;
    return header_.count("Connection") && header_.find("Connection")->second == "Keep-Alive" && version_ == "1.1"; 
}

bool HttpRequest::parse(Buffer &buff){
    const char CRLF[] = "\r\n";
    if(buff.ReadableBytes() <= 0){
        return false;
    }
    std::string t = buff.GetAllToStr();
    buff.Append(t);
    printf("req\n  %s \n\n",     t.c_str());
    while(buff.ReadableBytes() && state_ != PARSE_STATE::FINISH){
        const char *lineEnd = search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF+2);
        string line(buff.Peek(), lineEnd);
      //  printf("line:%s\n", line.c_str());
        switch (state_)
        {
        case PARSE_STATE::REQUEST_LINE:
            if(!ParseRequestLine_(line)){
                return false;
            } 
            ParsePath_();
            break;
        case PARSE_STATE::HEADERS:
            ParseHeader_(line);
            if(buff.ReadableBytes() <= 2){
                state_ = PARSE_STATE::FINISH;
            }
            break;
        case PARSE_STATE::BODY:
            ParseBody_(line);
            break;
        default:
            break;
        }
        if(lineEnd == buff.BeginWrite()){
            break;
        }
        buff.GetUntil(lineEnd + 2); 
    }
     
   // printf("[%s], [%s], [%s]\n", method_.c_str(), path_.c_str(), version_.c_str());
    //LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

bool HttpRequest::ParsePath_(){
    if(path_ == "/"){
        path_ = "/index.html";
        return true;
    }else{
        if(HTML_.count(path_) && method_ != "POST"){
            path_ += ".html";
            return true;
        }
    }
    return false;
}
bool HttpRequest::ParseRequestLine_(const string &line){
    regex patten("([^ ]*) ([^ ]*) HTTP/([^ ]*)");
    smatch match;
    if(regex_match(line, match, patten)){
        method_ = match[1];
        path_ = match[2];
        version_ = match[3];
        state_ = PARSE_STATE::HEADERS;
        return true;
    }
    return false;
}
void HttpRequest::ParseHeader_(const string &line){
    regex patten("([^:]*): (.*)");
    smatch match;
    if(regex_match(line, match, patten)){
        header_[match[1]] = match[2]; 
    }else{
        state_ = PARSE_STATE::BODY;
    }
}
void HttpRequest::ParseBody_(const string &line){
    body_ = line;
    ParsePost_();
    state_ = PARSE_STATE::FINISH;
}

void HttpRequest::ParsePost_(){
	
  //  std::cout<<body_<<"====\n";
    std::cout<< (method_ == "POST")<<"\n";
    if(method_ == "POST" /*&& header_["Content-Type"] == "application/x-www-form-urlencoded"*/){
        printf("here\n");
        std::cout<<path_<<"\n";

        if(path_ == "/login"){
            
            regex patten("([^=]*)=([^&]*)&([^=]*)=([^&]*)");
            smatch match;
            if(regex_match(body_, match, patten)){
        	    post_[match[1]] = match[2];
        	    post_[match[3]] = match[4];
		    }
            bool getSuccess = false;
            bool check = false;
            try{
                std::string pwd;
                getSuccess = tableManager->GetValue("user", stoul(post_["usernum"]), "pwd", pwd);
                if(pwd == post_["password"]){
                    check = true;
                }
                printf("pwd1:%s, pwd2:%s check%d\n", pwd.c_str(), post_["password"].c_str(), check);
            } catch(...){
                printf("get fail in req\n");
                getSuccess = false;
                check = false;
            }
            
            if(getSuccess && check){
                path_ = "/index.html";
            }else{
                path_ = "/login.html";
            }

        }else if(path_ == "/register"){
            //num=1&name=2&pwd=3
            regex patten("([^=]*)=([^&]*)&([^=]*)=([^&]*)&([^=]*)=([^&]*)");
            smatch match;
            if(regex_match(body_, match, patten)){
        	    post_[match[1]] = match[2];
        	    post_[match[3]] = match[4];
                post_[match[5]] = match[6];
		    }
            bool insertSuccess = false;
            try{
                std::vector<std::string> values;
                values.push_back(post_["usernum"]);
                values.push_back(post_["username"]);
                values.push_back(post_["password"]);
                insertSuccess = tableManager->Insert("user", values);

            } catch(...){
                printf("get fail to get in req\n");
                insertSuccess = false;
            }
            if(insertSuccess){
                path_ = "/login.html";
            }
        }
    	
    }
}

std::string HttpRequest::path() const{
    return path_;
}

std::string& HttpRequest::path(){
    return path_;
}
std::string HttpRequest::method() const {
    return method_;
}

std::string HttpRequest::version() const {
    return version_;
}

std::string HttpRequest::GetPost(const std::string& key) const {
    assert(key != "");
    if(post_.count(key)) {
        return post_.find(key)->second;
    }
    return "";
}

std::string HttpRequest::GetPost(const char* key) const {
    assert(key != nullptr);
    if(post_.count(key)) {
        return post_.find(key)->second;
    }
    return "";
}

