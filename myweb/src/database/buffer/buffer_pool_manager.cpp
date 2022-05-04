#include"../a-include/buffer/buffer_pool_manager.h"
#include"assert.h"
#include"iostream"

namespace bustub{
    BufferPoolManager::BufferPoolManager(size_t poolSize, DiskManager *diskManager)
    : poolSize_(poolSize), diskManager_(diskManager){
        pages_ = new Page[poolSize_];
        replacer_ = new LRUReplacer(poolSize);
        int n = static_cast<int>(poolSize);
        for(int i = 0; i < n; i++){
            freeList_.emplace_back(i);
        }
    }
    BufferPoolManager::~BufferPoolManager(){
        delete[] pages_;
        delete replacer_;
    }
    Page *BufferPoolManager::FetchPageImpl(page_id_t pageId){
        std::lock_guard<std::mutex> guard(mtx_);
        auto it = pageTable_.find(pageId);
        frame_id_t frameId = -1;
        if(it != pageTable_.end()){
            frameId = it->second;
            replacer_->Pin(frameId);
            Page *page = &pages_[frameId];
            page->pinCount_++;
            return page;

        }else if(!freeList_.empty()){
            frameId = freeList_.back();
            freeList_.pop_back();
            Page *page = InitPage(frameId, pageId, false);
            return page;
        }else if(replacer_->Victim(&frameId)){
            assert(frameId != -1);
            VictimByFrameId(frameId);
            Page *page = InitPage(frameId, pageId, false);
            return page;

        }else{
            printf("fail to fetch\n");

        }
        throw std::runtime_error("bpm is full\n");
        return nullptr;

    }
    void BufferPoolManager::VictimByFrameId(int frameId){
        Page *vicPage = &pages_[frameId];
        int vicPageId = vicPage->pageId_;
        if(vicPage->isDirty_){
            char *data = pages_[frameId].data_;
            diskManager_->WritePage(vicPageId, data);
            //write 不是立即执行的 不解锁应该也可以
        }
        pageTable_.erase(vicPageId);
    }
    //false if the page pin count is <= 0 before this call, true otherwise
    bool BufferPoolManager::UnpinPageImpl(page_id_t pageId, bool isDirty){
        std::lock_guard<std::mutex> guard(mtx_);
        auto it = pageTable_.find(pageId);
        if(it == pageTable_.end()){
            return false;
        }
        int frameId = it->second;
        Page *page = &pages_[frameId];
        if(isDirty){
            page->isDirty_ = true;
        }
        if(page->pinCount_ <= 0){
            return false;
        }
        if(--page->pinCount_ == 0){
            replacer_->Unpin(frameId);
        }
        return true;

    }
    bool BufferPoolManager::FlushPageImpl(page_id_t pageId){
        std::unique_lock<std::mutex> lock(mtx_);
        auto it = pageTable_.find(pageId);
        if(it == pageTable_.end() || pageId == INVALID_PAGE_ID){
            return false;
        }
        Page *page = &pages_[it->second];
        if(!page->isDirty_) return false;
        page->isDirty_ = false;
        lock.unlock();
        diskManager_->WritePage(pageId, page->data_);
      //  std::cout<<"flush "<< pageId<<" to disk\n";
        return true;

    }

    Page *BufferPoolManager::NewPageImpl(page_id_t *pageId){
        std::unique_lock<std::mutex> lock(mtx_);
        frame_id_t frameId = -1;
        page_id_t newPageId = diskManager_->AllocatePage();
        *pageId = newPageId;
     //   printf("new pageid %d\n", newPageId);
        bool allPined = true;
        int n = static_cast<int>(poolSize_);
        for(int i = 0; i < n; i++){
            if(pages_[i].pinCount_ == 0){
                allPined = false;
                break;
            }
        }
        if(allPined){
           throw std::runtime_error("bpm is full\n");
        }
        if(!freeList_.empty()){
            frameId = freeList_.back();
            freeList_.pop_back();
            Page *page = InitPage(frameId, newPageId, true);
            return page;
        }else if(replacer_->Victim(&frameId)){
            assert(frameId != -1);
            VictimByFrameId(frameId);
            Page *page = InitPage(frameId, newPageId, true);
            return page;
        }
        throw std::runtime_error("bpm is full\n");
        return nullptr;
    }
    Page *BufferPoolManager::InitPage(frame_id_t frameId, page_id_t pageId, bool isCreate){
        Page *page = &pages_[frameId];
        replacer_->Pin(frameId);
        page->pinCount_ = 1;
        page->pageId_ = pageId;
        page->isDirty_ = false;
        pageTable_[pageId] = frameId;
        if(isCreate){
            diskManager_->WritePage(pageId, page->data_);
        }else{
            diskManager_->ReadPage(pageId, page->data_);
        }
        return page;
    };
    bool BufferPoolManager::DeletePageImpl(page_id_t page_id){
        return false;
    }
    void BufferPoolManager::FlushAllPagesImpl() {
       // std::lock_guard<std::mutex> guard(mtx_);
        for(auto i : pageTable_){
  	        FlushPageImpl(i.first);
        }
    }

}

