#pragma once
#include<mutex>
#include"lru_replacer.h"
#include"../storage/disk/disk_manager.h"
#include"../storage/page/page.h"

namespace bustub{

class BufferPoolManager{
public:
    enum class CallBackType {BEFORE, AFTER};
    using bufferPoolCallBackF = void(*)(enum CallBackType, const page_id_t page_id);

    BufferPoolManager(size_t poolSize, DiskManager *diskManager);

    ~BufferPoolManager();

    Page *FetchPage(page_id_t page_id){
         auto *ret = FetchPageImpl(page_id);
        return ret;
    }

    Page *FetchPageImpl(page_id_t page_id);

    bool UnpinPage(page_id_t page_id, bool isDirty){
        auto ret = UnpinPageImpl(page_id, isDirty);
        return ret;
    }

    bool UnpinPageImpl(page_id_t page_id, bool isDirty);

            
    bool FlushPage(page_id_t page_id){
        auto ret =  FlushPageImpl(page_id);
        return ret;
    }

    bool FlushPageImpl(page_id_t page_id);

    Page *NewPage(page_id_t *page_id){
        auto *ret = NewPageImpl(page_id);
        return ret;
    }

    Page *NewPageImpl(page_id_t *page_id);

    bool DeletePage(page_id_t page_id){
        auto ret = DeletePageImpl(page_id);
        return ret;
    }

    bool DeletePageImpl(page_id_t page_id);

    void FlushAllPages(){
        FlushAllPagesImpl();
    }

    void FlushAllPagesImpl();

    Page *GetPages(){
        return pages_;
    }
    size_t GetPoolSize(){
        return poolSize_;
    }

    void VictimByFrameId(int frameId);

    Page *InitPage(frame_id_t frame_id, page_id_t page_id, bool isCreate);

protected:
    size_t poolSize_;
    Page *pages_;
    DiskManager *diskManager_;
    std::unordered_map<page_id_t, frame_id_t> pageTable_;
    LRUReplacer *replacer_;
    std::list<frame_id_t> freeList_;
    std::mutex mtx_;
};

}
