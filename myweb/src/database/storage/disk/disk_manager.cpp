#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <string>
#include <thread>  // NOLINT


#include <unistd.h>
#include "../../a-include/storage/disk/disk_manager.h"

namespace bustub{
	DiskManager::DiskManager(const std::string &dbFile)
	: fileName_(dbFile), nextPageId_(0), numFlushes_(0), numWrites_(0), flushLog_(false), flushLogF_(nullptr){

		dbIo_.open(dbFile, std::ios::binary | std::ios::in | std::ios::out);
		if(!dbIo_.is_open()){
			dbIo_.clear();
			dbIo_.open(dbFile, std::ios::binary | std::ios::trunc | std::ios::out);
			dbIo_.close();
			dbIo_.open(dbFile, std::ios::binary | std::ios::out | std::ios::in);
			if(!dbIo_.is_open()){
				printf("can not open db file\n");
				return;	
			}
		}
	}
	void DiskManager::ShutDown(){
		dbIo_.close();
	}
	void DiskManager::WritePage(page_id_t pageId, const char *pageData){
		size_t offset = pageId * PAGE_SIZE;
		numWrites_++;
		dbIo_.seekp(offset);	
		dbIo_.write(pageData, PAGE_SIZE);
		if(dbIo_.bad()){
			printf("db fail to write\n");
			return;
		}
		dbIo_.flush();
	}
	void DiskManager::ReadPage(page_id_t pageId, char *pageData){
		ssize_t offset = pageId * PAGE_SIZE;
		if(offset > GetFileSize(fileName_)){
			printf("Size:%d, fileName:%s\n", GetFileSize(fileName_), fileName_.c_str());
			
			printf("pageid :%d read out of write\n", pageId);
		}else{
			dbIo_.seekp(offset);
			dbIo_.read(pageData, PAGE_SIZE);
			if(dbIo_.bad()){
				printf("db fail to read\n");
				dbIo_.clear();
			}
			int readCount = dbIo_.gcount();
			if(readCount < PAGE_SIZE){
				printf("read less than size\n");
				dbIo_.clear();
				memset(pageData + readCount, 0, PAGE_SIZE - readCount);
			}
		}
		
	}
	page_id_t DiskManager::AllocatePage(){
		return nextPageId_++;
	}
	int DiskManager::GetNumWtries() const {
		return numWrites_;
	}
	//int DiskManager::GetFlushState()
	int DiskManager::GetNumFlushes() const{
		return numFlushes_;
	}
	int DiskManager::GetFileSize(const std::string &fileName){
		struct stat stat_buf;
		int rc = stat(fileName.c_str(), &stat_buf);
	//	printf("%d\n", rc);
		return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
	}
	
	
}


