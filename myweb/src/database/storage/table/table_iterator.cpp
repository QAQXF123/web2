
#include <cassert>

#include "../../a-include/storage/table/table_heap.h"

namespace bustub {

TableIterator::TableIterator(TableHeap *table_heap, RID rid)
    : table_heap_(table_heap), tuple_(new Tuple(rid)){
  if (rid.GetPageId() != INVALID_PAGE_ID) {
    table_heap_->GetTuple(tuple_->rid_, tuple_);
  }
}

const Tuple &TableIterator::operator*() {
  assert(*this != table_heap_->End());
  return *tuple_;
}

Tuple *TableIterator::operator->() {
  assert(*this != table_heap_->End());
  return tuple_;
}

TableIterator &TableIterator::operator++() {
  BufferPoolManager *buffer_pool_manager = table_heap_->buffer_pool_manager_;
  auto cur_page = static_cast<TablePage *>(buffer_pool_manager->FetchPage(tuple_->rid_.GetPageId()));
  cur_page->RLock();
  assert(cur_page != nullptr);  // all pages are pinned

  RID next_tuple_rid;
  if (!cur_page->GetNextTupleRid(tuple_->rid_, &next_tuple_rid)) {  // end of this page
    while (cur_page->GetNextPageId() != INVALID_PAGE_ID) {
      auto next_page = static_cast<TablePage *>(buffer_pool_manager->FetchPage(cur_page->GetNextPageId()));
      cur_page->RUlock();
      buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
      cur_page = next_page;
      cur_page->RLock();
      if (cur_page->GetFirstTupleRid(&next_tuple_rid)) {
        break;
      }
    }
  }
  tuple_->rid_ = next_tuple_rid;

  if (*this != table_heap_->End()) {
    table_heap_->GetTuple(tuple_->rid_, tuple_);
  }
  // release until copy the tuple
  cur_page->RUlock();
  buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
  return *this;
}

TableIterator TableIterator::operator++(int) {
  TableIterator clone(*this);
  ++(*this);
  return clone;
}

}  // namespace bustub
