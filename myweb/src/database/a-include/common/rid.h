#pragma once

#include <iostream>
#include <cstdint>
#include <sstream>
#include <string>
#include <cstring>
#include "config.h"
namespace bustub{
    class RID{
        public:
            RID() = default;
            RID(page_id_t page_id, uint32_t slot_num) : pageId_(page_id), slotNum_(slot_num) {}
            explicit RID(int64_t rid) :pageId_(static_cast<page_id_t>(rid>>32)), slotNum_(static_cast<uint32_t>(rid)){}
            inline int64_t Get() const{
                return (static_cast<int64_t>(pageId_)) << 32 | slotNum_; 
            }
            inline page_id_t GetPageId() const {
                return pageId_;
            }
            inline uint32_t GetSlotNum() const {
                return slotNum_;
            }
            inline void Set(page_id_t pageId, uint32_t slotNum){
                pageId_ = pageId;
                slotNum_ = slotNum;
            }
            inline std::string ToString() const{
                std::string ret = "pageId "+ std::to_string(pageId_) + " slotNum" + std::to_string(slotNum_) + "\n";
                return ret;
            }
            bool operator==(const RID &rhs){
                return pageId_ == rhs.pageId_ && slotNum_ == rhs.slotNum_;
            }
            private:
                page_id_t pageId_{INVALID_PAGE_ID};
                uint32_t slotNum_{0};

    };
}
