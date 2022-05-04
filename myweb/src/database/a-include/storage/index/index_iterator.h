
/**
 * index_iterator.h
 * For range scan of b+ tree
 */
#pragma once
#include "../page/b_plus_tree_leaf_page.h"

namespace bustub {

#define INDEXITERATOR_TYPE IndexIterator<KeyType, ValueType, KeyComparator>

INDEX_TEMPLATE_ARGUMENTS
class IndexIterator {
 public:
  // you may define your own constructor based on your member variables
  IndexIterator();
  IndexIterator(Page* page_ptr, int index, BufferPoolManager* bpm_ptr);
  ~IndexIterator();

  bool isEnd();

  const MappingType &operator*();

  IndexIterator &operator++();

  bool operator==(const IndexIterator &itr) const { 
    return page_id_ == itr.page_id_ && index_ == itr.index_;

   }

  bool operator!=(const IndexIterator &itr) const { 
    return page_id_ != itr.page_id_ || index_ != itr.index_;
   }
  B_PLUS_TREE_LEAF_PAGE_TYPE* SafelyGetAndLatchLeafPage();

 private:
  page_id_t page_id_;
  int index_;
  Page* page_ptr_;
  B_PLUS_TREE_LEAF_PAGE_TYPE* leaf_ptr_;
  BufferPoolManager* buffer_pool_manager_;
  // add your own private member variables here
};

}  // namespace bustub
