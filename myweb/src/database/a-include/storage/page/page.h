#pragma once

#include<cstring>
#include<iostream>

#include"../../common/config.h"
#include"../../common/rwlock.h"

namespace bustub{
    class Page{
        friend class BufferPoolManager;

        public:
            Page(){
                ResetMemory();
            }
            ~Page() = default;

            inline char *GetData(){
                return data_;
            }

            inline page_id_t GetPageId(){
                return pageId_;
            }

            inline int GetPinCount(){
                return pinCount_;
            }

            inline bool IsDirty(){
                return isDirty_;
            }

            inline void SetDirty(){
                isDirty_ = true;
            }

            inline void WLock(){
                rwlock_.WLock();
            }

            inline void WUlock(){
                rwlock_.WUnlock();
            }

            inline void RLock(){
                rwlock_.RLock();
            }
            
            inline void RUlock(){
                rwlock_.RUnlock();
            }


        protected:
            static constexpr size_t SIZE_PAGE_HEADER = 8;
            static constexpr size_t OFFSET_PAGE_START = 0;
            //static constexpr size_t OFFSET_LSN = 4;
        private:
            inline void ResetMemory(){
                memset(data_, OFFSET_PAGE_START, PAGE_SIZE);
            }
            char data_[PAGE_SIZE]{};
            page_id_t pageId_ = INVALID_PAGE_ID;
            int pinCount_ = 0;
            bool isDirty_ = false;
            RWlock rwlock_;

    };
}
