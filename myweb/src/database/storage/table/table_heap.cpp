//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_heap.cpp
//
// Identification: src/storage/table/table_heap.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include "../../a-include/storage/table/table_heap.h"

namespace bustub {

TableHeap::TableHeap(BufferPoolManager *buffer_pool_manager, page_id_t first_page_id, page_id_t last_page_id)
    : buffer_pool_manager_(buffer_pool_manager),
      first_page_id_(first_page_id), last_page_id_(last_page_id) {}

TableHeap::TableHeap(BufferPoolManager *buffer_pool_manager)
    : buffer_pool_manager_(buffer_pool_manager){
  // Initialize the first table page.
  TablePage *first_page = reinterpret_cast<TablePage *>(buffer_pool_manager_->NewPage(&first_page_id_));
  first_page->WLock();
  first_page->Init(first_page_id_, PAGE_SIZE, INVALID_PAGE_ID);
  last_page_id_ = first_page_id_;
  first_page->WUlock();
  buffer_pool_manager_->UnpinPage(first_page_id_, true);
}

bool TableHeap::InsertTuple(const Tuple &tuple, RID *rid) {
 // printf("tuple size in insert:%d\n", tuple.size_);
  if (tuple.size_ + 32 > PAGE_SIZE) {  // larger than one page size
    return false;
  }

  auto cur_page = static_cast<TablePage *>(buffer_pool_manager_->FetchPage(last_page_id_));
  if (cur_page == nullptr) {
    return false;
  }
  //printf("table heap insert tuple\n");

  cur_page->WLock();
  // Insert into the first page with enough space. If no such page exists, create a new page and insert into that.
  // INVARIANT: cur_page is WLatched if you leave the loop normally.
  while (!cur_page->InsertTuple(tuple, rid)) {
    auto next_page_id = cur_page->GetNextPageId();
    // If the next page is a valid page,
    if (next_page_id != INVALID_PAGE_ID) {
      // Unlatch and unpin the current page.
      cur_page->WUlock();
      buffer_pool_manager_->UnpinPage(cur_page->GetTablePageId(), false);
      // And repeat the process with the next page.
      cur_page = static_cast<TablePage *>(buffer_pool_manager_->FetchPage(next_page_id));
      cur_page->WLock();
    } else {
      // Otherwise we have run out of valid pages. We need to create a new page.
      auto new_page = static_cast<TablePage *>(buffer_pool_manager_->NewPage(&next_page_id));
      // If we could not create a new page,
      if (new_page == nullptr) {
        // Then life sucks and we abort the transaction.
        cur_page->WUlock();
        buffer_pool_manager_->UnpinPage(cur_page->GetTablePageId(), false);
        return false;
      }
      // Otherwise we were able to create a new page. We initialize it now.
      new_page->WLock();
      cur_page->SetNextPageId(next_page_id);
      new_page->Init(next_page_id, PAGE_SIZE, cur_page->GetTablePageId());
      cur_page->WUlock();
      buffer_pool_manager_->UnpinPage(cur_page->GetTablePageId(), true);
      cur_page = new_page;
    }
    
  }
  // This line has caused most of us to double-take and "whoa double unlatch".
  // We are not, in fact, double unlatching. See the invariant above.
  cur_page->WUlock();
  buffer_pool_manager_->UnpinPage(cur_page->GetTablePageId(), true);
  last_page_id_ = cur_page->GetTablePageId();
  // Update the transaction's write set.
  return true;
}

bool TableHeap::MarkDelete(const RID &rid) {
  // TODO(Amadou): remove empty page
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  // If the page could not be found, then abort the transaction.
  if (page == nullptr) {
    return false;
  }
  // Otherwise, mark the tuple as deleted.
  page->WLock();
  page->MarkDelete(rid);
  page->WUlock();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
  // Update the transaction's write set.
  return true;
}

bool TableHeap::UpdateTuple(const Tuple &tuple, const RID &rid) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  // If the page could not be found, then abort the transaction.
  if (page == nullptr) {
    return false;
  }
  // Update the tuple; but first save the old value for rollbacks.
  Tuple old_tuple;
  page->WLock();
  bool is_updated = page->UpdateTuple(tuple, &old_tuple, rid);
  page->WUlock();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), is_updated);
  // Update the transaction's write set.
  return is_updated;
}

void TableHeap::ApplyDelete(const RID &rid) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
 // BUSTUB_ASSERT(page != nullptr, "Couldn't find a page containing that RID.");
  // Delete the tuple from the page.
  page->WLock();
  page->ApplyDelete(rid);
  page->WUlock();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

void TableHeap::RollbackDelete(const RID &rid) {
  // Find the page which contains the tuple.
  auto page = reinterpret_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  //BUSTUB_ASSERT(page != nullptr, "Couldn't find a page containing that RID.");
  // Rollback the delete.
  page->WLock();
  page->WUlock();
  buffer_pool_manager_->UnpinPage(page->GetTablePageId(), true);
}

bool TableHeap::GetTuple(const RID &rid, Tuple *tuple) {
  // Find the page which contains the tuple.
 // printf("here\n");
  auto page = static_cast<TablePage *>(buffer_pool_manager_->FetchPage(rid.GetPageId()));
  //std::cout<<"page id:" << rid.GetPageId() <<"\n";
  // If the page could not be found, then abort the transaction.
  if (page == nullptr) {
    return false;
  }
  // Read the tuple from the page.
  page->RLock();
  //printf("pageid:%d, slotid:%d\n", rid.GetPageId(), rid.GetSlotNum());
  bool res = page->GetTuple(rid, tuple);
  //printf("get tuple %d!\n", res);
  page->RUlock();
  buffer_pool_manager_->UnpinPage(rid.GetPageId(), false);
  return res;
}

TableIterator TableHeap::Begin() {
  // Start an iterator from the first page.
  // TODO(Wuwen): Hacky fix for now. Removing empty pages is a better way to handle this.
  RID rid;
  auto page_id = first_page_id_;
  while (page_id != INVALID_PAGE_ID) {
    auto page = static_cast<TablePage *>(buffer_pool_manager_->FetchPage(page_id));
    page->RLock();
    // If this fails because there is no tuple, then RID will be the default-constructed value, which means EOF.
    auto found_tuple = page->GetFirstTupleRid(&rid);
    page->RUlock();
    buffer_pool_manager_->UnpinPage(page_id, false);
    if (found_tuple) {
      break;
    }
    page_id = page->GetNextPageId();
  }
  return TableIterator(this, rid);
}

TableIterator TableHeap::End() { return TableIterator(this, RID(INVALID_PAGE_ID, 0)); }

}  // namespace bustub
