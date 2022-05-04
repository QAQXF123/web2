
#include "../a-include/storage/index/b_plus_tree_index.h"

namespace bustub {
/*
 * Constructor
 */
INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_INDEX_TYPE::BPlusTreeIndex(IndexMetadata *metadata, BufferPoolManager *buffer_pool_manager)
    : Index(metadata),
      comparator_(metadata->GetKeySchema()),
      container_(metadata->GetName(), buffer_pool_manager, comparator_) {}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_INDEX_TYPE::InsertEntry(const Tuple &key, RID rid) {
  // construct insert index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.Insert(index_key, rid);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_INDEX_TYPE::DeleteEntry(const Tuple &key, RID rid) {
  // construct delete index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.Remove(index_key);
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_INDEX_TYPE::ScanKey(const Tuple &key, std::vector<RID> *result) {
  // construct scan index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.GetValue(index_key, result);
}

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_INDEX_TYPE::GetBeginIterator() { return container_.begin(); }

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_INDEX_TYPE::GetBeginIterator(const KeyType &key) { return container_.Begin(key); }

INDEX_TEMPLATE_ARGUMENTS
INDEXITERATOR_TYPE BPLUSTREE_INDEX_TYPE::GetEndIterator() { return container_.end(); }

template class BPlusTreeIndex<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTreeIndex<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTreeIndex<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTreeIndex<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTreeIndex<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub