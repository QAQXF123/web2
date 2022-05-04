#pragma once
#include"../buffer/buffer.h"

#include"../database/main/table_manager.h"
#include<unordered_map>
#include<unordered_set>
#include <regex>
#include<iostream>
class HttpRequest{
    public:
        enum class PARSE_STATE{
            REQUEST_LINE,
            HEADERS,
            BODY,
            FINISH,
        };
        enum class HTTP_CODE{
            NO_REQUEST,
            GET_REQUEST,
            BAD_REQUEST,
            NO_RESOURSE,
            FORBIDDENT_REQUEST,
            FIEL_REQUEST,
            INTERNAL_ERROR,
            CLOSED_CONNECTION,
        };

        HttpRequest(){
            Init();
        }
        ~HttpRequest() = default;

        void Init();

        bool parse(Buffer& buff);

        std::string path() const;
        std::string& path();
        std::string method() const;
        std::string version() const;
        std::string GetPost(const std::string &key) const;
        std::string GetPost(const char *key) const;
        bool IsKeepAlive() const;

    public:  
    //private:
        bool ParseRequestLine_(const std::string &line);
        void ParseHeader_(const std::string &line);
        void ParseBody_(const std::string &line);
        bool ParsePath_();
        void ParsePost_();


        void ParseFromUrlencoded_();

        static bool UserVerfify(const std::string &name, const std::string &pwd, bool isLogin);
        
        PARSE_STATE state_;

        std::string method_, path_, version_, body_;
        std::unordered_map<std::string, std::string> header_;
        std::unordered_map<std::string, std::string> post_;

        static const std::unordered_set<std::string> HTML_;
    	static const std::unordered_map<std::string, int> HTML_TAG_;

        static bustub::TableManager *tableManager;
    

};
