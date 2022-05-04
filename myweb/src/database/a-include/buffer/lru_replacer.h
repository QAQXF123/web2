#pragma once
#include "../common/config.h"
#include <unordered_map>
#include <list>
#include <mutex>
#include<iterator>
namespace bustub{
    class LRUReplacer{
        public:
            LRUReplacer(size_t mumPages);
            ~LRUReplacer();

            bool Victim(frame_id_t *frame_id);

            void Pin(frame_id_t frame_id);

            void Unpin(frame_id_t frame_id);

            size_t Size();
        private:
            std::unordered_map<frame_id_t, std::list<frame_id_t>::iterator> valueMap_;
            std::list<frame_id_t> accessList_;
            std::mutex mtx_;
            size_t capacity_;
    };

}