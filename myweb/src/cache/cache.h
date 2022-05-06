#pragma once
#include <unordered_map>
#include <mutex>
#include <list>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <iostream>
#include <assert.h>
#include <sys/mman.h>    // mmap, munmap
class Cache{
	public:
		struct mmFileNode{
			
			mmFileNode(std::string path, char *mmFile, long length): 
			path(path), mmFile(mmFile), length(length){}
			
			std::string path;
			char *mmFile;
			long length;
		};
        
		Cache(int maxSize = 1): maxSize_(maxSize){
            
        }
		~Cache(){
            auto it = mmFileMap_.begin();
			while(it != mmFileMap_.end()){
                mmFileNode *node = *it->second;
                UnMapFile_(node->mmFile, node->length);
            }
		}
		
		/*void look(){
			for(auto i : mmFileMap_){
				printf("%s\n", (i.first).c_str());
			}
		}*/
	
		mmFileNode* GetmmFileNode(std::string path, long length){
			std::unique_lock<std::mutex> locker(mtx_);

			if(mmFileMap_.count(path)){	
				auto it = mmFileMap_[path];
				auto node = *it;
				nodeList_.erase(it);
				nodeList_.push_front(node);
				mmFileMap_[path] = nodeList_.begin();
				return node;
				
			}else{
    			mmFileNode *node = new mmFileNode(path, nullptr, length);
				locker.unlock();
				if(!PutNode_(path, node)){
					return nullptr;
				}else{
					return node;
				}
			} 	
		}
	private:	

		void UnMapFile_(char *mmFile, long length){
			if(mmFile){
				munmap(mmFile, length);
        		mmFile = nullptr;
			}
		}
		
		bool PutNode_(std::string path, mmFileNode *node){
			std::unique_lock<std::mutex> locker(mtx_);
			locker.unlock();
			int srcFd = open(path.data(), O_RDONLY);
			if(srcFd < 0) return false;
			int *mmRet = (int*)mmap(0, /*mmFileStat_.st_size*/node->length, PROT_READ, MAP_PRIVATE, srcFd, 0);
			if(*mmRet == -1) return false;
			node->mmFile = std::move((char*)(mmRet));
			close(srcFd);

			locker.lock();

			nodeList_.push_front(node);
			mmFileMap_[path] = nodeList_.begin();
			if(static_cast<int>(nodeList_.size()) > maxSize_){
				PopNode_();
			}
			return true;
		}
		void PopNode_(){
			mmFileMap_.erase(nodeList_.back()->path);
			mmFileNode *back = nodeList_.back();
			nodeList_.pop_back();
			UnMapFile_(back->mmFile, back->length);
			delete back;
		}
		
		int maxSize_;
		std::list<mmFileNode*> nodeList_;
		std::unordered_map<std::string, std::list<mmFileNode*>::iterator> mmFileMap_;	
		std::mutex mtx_;	
};