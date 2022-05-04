
#pragma once

#include <cassert>
#include "../../common/rid.h"
#include "tuple.h"

namespace bustub {

class TableHeap;

/**
 * TableIterator enables the sequential scan of a TableHeap.
 */
class TableIterator {
  friend class Cursor;

 public:
  TableIterator(TableHeap *table_heap, RID rid);

  TableIterator(const TableIterator &other)
      : table_heap_(other.table_heap_), tuple_(new Tuple(*other.tuple_)) {}

  ~TableIterator() { delete tuple_; }

  inline bool operator==(const TableIterator &itr) const { return tuple_->rid_.Get() == itr.tuple_->rid_.Get(); }

  inline bool operator!=(const TableIterator &itr) const { return !(*this == itr); }

  const Tuple &operator*();

  Tuple *operator->();

  TableIterator &operator++();

  TableIterator operator++(int);

  TableIterator &operator=(const TableIterator &other) {
    table_heap_ = other.table_heap_;
    *tuple_ = *other.tuple_;
    return *this;
  }

 private:
  TableHeap *table_heap_;
  Tuple *tuple_;
};

}  // namespace bustub
