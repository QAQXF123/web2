
#pragma once

#include <map>
#include <string>
#include <vector>

#include "b_plus_tree.h"
#include "index.h"

namespace bustub {

#define BPLUSTREE_INDEX_TYPE BPlusTreeIndex<KeyType, ValueType, KeyComparator>

INDEX_TEMPLATE_ARGUMENTS
class BPlusTreeIndex : public Index {
 public:
  BPlusTreeIndex(IndexMetadata *metadata, BufferPoolManager *buffer_pool_manager);

  void InsertEntry(const Tuple &key, RID rid) override;

  void DeleteEntry(const Tuple &key, RID rid) override;

  void ScanKey(const Tuple &key, std::vector<RID> *result) override;

  INDEXITERATOR_TYPE GetBeginIterator();

  INDEXITERATOR_TYPE GetBeginIterator(const KeyType &key);

  INDEXITERATOR_TYPE GetEndIterator();

 protected:
  // comparator for key
  KeyComparator comparator_;
  // container
  BPlusTree<KeyType, ValueType, KeyComparator> container_;
};

}  // namespace bustub
