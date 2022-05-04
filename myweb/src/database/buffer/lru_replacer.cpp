#include"../a-include/buffer/lru_replacer.h"

namespace bustub{
    LRUReplacer::LRUReplacer(size_t numPages){
        capacity_ = numPages;
    }
    LRUReplacer::~LRUReplacer() = default;

    bool LRUReplacer::Victim(frame_id_t *frame_id){
        mtx_.lock();
        if(accessList_.empty()){
            mtx_.unlock();
            return false;
        }
        auto victim = accessList_.back();
        valueMap_.erase(victim);
        accessList_.pop_back();
        *frame_id = victim;
        mtx_.unlock();
        return true;
    }

    void LRUReplacer::Pin(frame_id_t frame_id){
        mtx_.lock();
        if(valueMap_.count(frame_id)){
            accessList_.erase(valueMap_[frame_id]);
            valueMap_.erase(frame_id);
        }
        mtx_.unlock();
    }
    void LRUReplacer::Unpin(frame_id_t frame_id){
        mtx_.lock();
        if(valueMap_.count(frame_id)){
            mtx_.unlock();
            return;
        }
        while(accessList_.size() >= capacity_){
            valueMap_.erase(accessList_.back());
            accessList_.pop_back();
        }
        accessList_.push_front(frame_id);
        valueMap_[frame_id] = accessList_.begin();
        mtx_.unlock();
    }
    size_t LRUReplacer::Size(){
        return accessList_.size();
    }
    
}