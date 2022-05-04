#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../buffer/buffer_pool_manager.h"
#include "schema.h"
#include "../storage/index/b_plus_tree_index.h"
#include "../storage/index/index.h"
#include "../storage/table/table_heap.h"

namespace bustub {

/**
 * Typedefs
 */
using table_oid_t = uint32_t;
using column_oid_t = uint32_t;
using index_oid_t = uint32_t;

/**
 * Metadata about a table.
 */
struct TableMetadata {
  TableMetadata(Schema schema, std::string name, TableHeap *table, table_oid_t oid)
      : schema_(std::move(schema)), name_(std::move(name)), table_(table), oid_(oid) {}
  Schema schema_;
  std::string name_;
  TableHeap *table_;
  table_oid_t oid_;
  page_id_t table_first_id_;
};

/**
 * Metadata about a index
 */
struct IndexInfo {
  IndexInfo(Schema key_schema, std::string name, std::unique_ptr<Index> &&index, index_oid_t index_oid,
            std::string table_name, size_t key_size)
      : key_schema_(std::move(key_schema)),
        name_(std::move(name)),
        index_(std::move(index)),
        index_oid_(index_oid),
        table_name_(std::move(table_name)),
        key_size_(key_size) {}
  Schema key_schema_;
  std::string name_;
  std::unique_ptr<Index> index_;
  index_oid_t index_oid_;
  std::string table_name_;
  const size_t key_size_;
};

/**
 * Catalog is a non-persistent catalog that is designed for the executor to use.
 * It handles table creation and table lookup.
 */
class Catalog {
 public:
  /**
   * Creates a new catalog object.
   * @param bpm the buffer pool manager backing tables created by this catalog
   * @param lock_manager the lock manager in use by the system
   * @param log_manager the log manager in use by the system
   */
  Catalog(BufferPoolManager *bpm)
      : bpm_{bpm} {}

  /**
   * Create a new table and return its metadata.
   * @param txn the transaction in which the table is being created
   * @param table_name the name of the new table
   * @param schema the schema of the new table
   * @return a pointer to the metadata of the new table
   */
  TableMetadata *CreateTable(const std::string &table_name, const Schema &schema) {
    table_oid_t table_oid = next_table_oid_++;
    TableHeap *table = new TableHeap(bpm_);
    TableMetadata *table_meta_data_ptr =  new TableMetadata(schema, table_name, table, table_oid);
    table_meta_data_ptr->table_first_id_ = table->GetFirstPageId();
   // std::unique_ptr<TableMetadata> table_meta_data_ptr(new TableMetadata(schema, table_name, table, table_oid));
    
  //  TableMetadata* ptr = table_meta_data_ptr.get();
   // tables_.insert({table_oid, std::move(table_meta_data_ptr)});
    tables_[table_oid] = table_meta_data_ptr;
    names_.insert({table_name, table_oid});
    index_names_.insert({table_name, std::unordered_map<std::string, index_oid_t>()});
    printf("catalog cnt of info schema %d\n",table_meta_data_ptr->schema_.cnt());
    return table_meta_data_ptr;
  }
  void SetBPM(BufferPoolManager *bpm){
    bpm_ = bpm;
  }

  /** @return table metadata by name */
  TableMetadata *GetTable(const std::string &table_name) {
    auto search = names_.find(table_name);
    if (search == names_.end()){
     // throw std::out_of_range("fail to get table by name");
    }
    table_oid_t table_oid = search->second;
  //  return tables_.find(table_oid)->second.get();
    return tables_[table_oid];
   }

  /** @return table metadata by oid */
  TableMetadata *GetTable(table_oid_t table_oid) { 
    if(!tables_.count(table_oid)){
      return nullptr;
    }
    return tables_[table_oid];

  }

  /**
   * Create a new index, populate existing data of the table and return its metadata.
   * @param txn the transaction in which the table is being created
   * @param index_name the name of the new index
   * @param table_name the name of the table
   * @param schema the schema of the table
   * @param key_schema the schema of the key
   * @param key_attrs key attributes
   * @param keysize size of the key
   * @return a pointer to the metadata of the new table
   */
  template <class KeyType, class ValueType, class KeyComparator>
  IndexInfo *CreateIndex(Transaction *txn, const std::string &index_name, const std::string &table_name,
                         const Schema &schema, const Schema &key_schema, const std::vector<uint32_t> &key_attrs,
                         size_t keysize) {

    index_oid_t index_oid = next_index_oid_++;
    // Do not use unique_ptr because when destruct object Index, Index will free metadata_ptr
    IndexMetadata *index_meta_data_ptr = new IndexMetadata(index_name, table_name, &schema, key_attrs);
    std::unique_ptr<Index> index_ptr(new BPLUSTREE_INDEX_TYPE(index_meta_data_ptr, bpm_));
    std::unique_ptr<IndexInfo> index_info_ptr(new IndexInfo(key_schema, index_name, std::move(index_ptr), index_oid, table_name, keysize));
    IndexInfo *ptr = index_info_ptr.get();
    //indexes_.insert({index_oid, std::move(index_info_ptr)});
    indexes_[index_oid] = ptr;
    auto search = index_names_.find(table_name);
    assert(search != index_names_.end());
    search->second.insert({index_name, index_oid});

    return ptr;      
  }

  IndexInfo *GetIndex(const std::string &index_name, const std::string &table_name) {
     auto search_table = index_names_.find(table_name);
    if (search_table == index_names_.end()){
      throw std::out_of_range("In GetIndex");
    }

    auto &index_table = search_table->second;
    auto search_index_oid = index_table.find(index_name);
    if (search_index_oid == index_table.end()){
      throw std::out_of_range("In GetIndex");
    }

    index_oid_t index_oid = search_index_oid->second;
    return indexes_[index_oid];
  }

  IndexInfo *GetIndex(index_oid_t index_oid) {
    auto search = indexes_.find(index_oid);
    if (search == indexes_.end()){
      throw std::out_of_range("In GetIndex");
    }
    return indexes_[index_oid];

   }


  std::vector<IndexInfo *> GetTableIndexes(const std::string &table_name) {
    std::vector<IndexInfo *> res{};
    auto search_table = index_names_.find(table_name);
    if (search_table == index_names_.end()){
      throw std::out_of_range("In GetTableIndexes");
    }

    auto index_set = search_table->second;
    for (const auto &pair: index_set){
      index_oid_t index_oid = pair.second;
      IndexInfo *index_info_ptr = indexes_[index_oid];
      res.push_back(index_info_ptr);
    }

    return res;
  }

 private:
  BufferPoolManager *bpm_;


  /** tables_ : table identifiers -> table metadata. Note that tables_ owns all table metadata. */
  std::unordered_map<table_oid_t, TableMetadata*> tables_;
  /** names_ : table names -> table identifiers */
  std::unordered_map<std::string, table_oid_t> names_;
  /** The next table identifier to be used. */
  std::atomic<table_oid_t> next_table_oid_{0};
  /** indexes_: index identifiers -> index metadata. Note that indexes_ owns all index metadata */
  std::unordered_map<index_oid_t, IndexInfo*> indexes_;
  /** index_names_: table name -> index names -> index identifiers */
  std::unordered_map<std::string, std::unordered_map<std::string, index_oid_t>> index_names_;
  /** The next index identifier to be used */
  std::atomic<index_oid_t> next_index_oid_{0};
};
}  // namespace bustub
