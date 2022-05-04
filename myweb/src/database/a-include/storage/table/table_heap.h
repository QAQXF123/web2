
#pragma once

#include "../../buffer/buffer_pool_manager.h"
#include "../page/table_page.h"
#include "table_iterator.h"
#include "tuple.h"

namespace bustub {

/**
 * TableHeap represents a physical table on disk.
 * This is just a doubly-linked list of pages.
 */
class TableHeap {
  friend class TableIterator;

 public:
  ~TableHeap() = default;

  /**
   * Create a table heap without a transaction. (open table)
   * @param buffer_pool_manager the buffer pool manager
   * @param lock_manager the lock manager
   * @param log_manager the log manager
   * @param first_page_id the id of the first page
   */
  TableHeap(BufferPoolManager *buffer_pool_manager,
            page_id_t first_page_id, page_id_t last_page_id);

  /**
   * Create a table heap with a transaction. (create table)
   * @param buffer_pool_manager the buffer pool manager
   * @param lock_manager the lock manager
   * @param log_manager the log manager
   * @param txn the creating transaction
   */
  TableHeap(BufferPoolManager *buffer_pool_manager);

  /**
   * Insert a tuple into the table. If the tuple is too large (>= page_size), return false.
   * @param tuple tuple to insert
   * @param[out] rid the rid of the inserted tuple
   * @param txn the transaction performing the insert
   * @return true iff the insert is successful
   */
  bool InsertTuple(const Tuple &tuple, RID *rid);

  /**
   * Mark the tuple as deleted. The actual delete will occur when ApplyDelete is called.
   * @param rid resource id of the tuple of delete
   * @param txn transaction performing the delete
   * @return true iff the delete is successful (i.e the tuple exists)
   */
  bool MarkDelete(const RID &rid);  // for delete

  /**
   * if the new tuple is too large to fit in the old page, return false (will delete and insert)
   * @param tuple new tuple
   * @param rid rid of the old tuple
   * @param txn transaction performing the update
   * @return true is update is successful.
   */
  bool UpdateTuple(const Tuple &tuple, const RID &rid);

  /**
   * Called on Commit/Abort to actually delete a tuple or rollback an insert.
   * @param rid rid of the tuple to delete
   * @param txn transaction performing the delete.
   */
  void ApplyDelete(const RID &rid);

  /**
   * Called on abort to rollback a delete.
   * @param rid rid of the deleted tuple.
   * @param txn transaction performing the rollback
   */
  void RollbackDelete(const RID &rid);

  /**
   * Read a tuple from the table.
   * @param rid rid of the tuple to read
   * @param tuple output variable for the tuple
   * @param txn transaction performing the read
   * @return true if the read was successful (i.e. the tuple exists)
   */
  bool GetTuple(const RID &rid, Tuple *tuple);

  /** @return the begin iterator of this table */
  TableIterator Begin();

  /** @return the end iterator of this table */
  TableIterator End();

  /** @return the id of the first page of this table */
  inline page_id_t GetFirstPageId() const { return first_page_id_; }
  inline page_id_t GetLastPageId() const { return last_page_id_; }

 private:
  BufferPoolManager *buffer_pool_manager_;
  page_id_t first_page_id_{};
  page_id_t last_page_id_{};
};

}  // namespace bustub
