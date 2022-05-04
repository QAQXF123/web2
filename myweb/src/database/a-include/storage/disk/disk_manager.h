#pragma once

#include<atomic>
#include<fstream>
#include<future>
#include<string>
#include"../../common/config.h"


namespace bustub{
	class DiskManager{
		public:
			DiskManager(const std::string &dbFile);
			~DiskManager() = default;
			void ShutDown();
			void WritePage(page_id_t pageId, const char *pageData);
			void ReadPage(page_id_t pageId, char *pageData);
			void WrtieLog(char *logData, int size);
			bool ReadLog(char *logData, int size, int offset);
			page_id_t AllocatePage();
			void DeallocaePage(page_id_t pageId);
			int GetNumFlushes() const;
			bool GetFlushState() const;
			int GetNumWtries() const;
			int GetFileSize(const std::string &fileName);
			
			inline void	SetFlushLogFuture(std::future<void> *f) { flushLogF_ = f;}
			inline bool HashFlushLogFuture(){ return flushLogF_ != nullptr;}
		private:
			
			std::fstream logIo_;
			std::string LogName_;
			
			std::fstream dbIo_;
			std::string fileName_;
			std::atomic<page_id_t> nextPageId_;
			int numFlushes_;
			int numWrites_;
			bool flushLog_;
			std::future<void> *flushLogF_;
			
			
	};
}
