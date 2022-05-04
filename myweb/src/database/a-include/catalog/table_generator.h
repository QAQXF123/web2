#pragma once

#include <utility>
#include <vector>

#include "catalog.h"
#include "../storage/table/table_heap.h"
#include "../buffer/buffer_pool_manager.h"

namespace bustub {

static constexpr uint32_t TEST1_SIZE = 1000;
static constexpr uint32_t TEST2_SIZE = 100;
static constexpr uint32_t TEST_VARLEN_SIZE = 10;

class TableGenerator {
 public:
  /**
   * Constructor
   */
  explicit TableGenerator(Catalog *catalog, BufferPoolManager *bpm) : 
  catalog_{catalog}, bpm_{bpm}{}

  Catalog *GetCatalog(){
    return catalog_;
  }
  BufferPoolManager *GetBufferPoolManager(){
    return bpm_;
  }

  /**
   * Generate test tables.
   */
  std::vector<std::pair<int64_t, RID>> GenerateTestTables();

 private:
  /**
   * Enumeration to characterize the distribution of values in a given column
   */
  enum class Dist : uint8_t { Uniform, Zipf_50, Zipf_75, Zipf_95, Zipf_99, Serial };

  /**
   * Metadata about the data for a given column. Specifically, the type of the
   * column, the distribution of values, a min and max if appropriate.
   */
  struct ColumnInsertMeta {
    /**
     * Name of the column
     */
    const char *name_;
    /**
     * Type of the column
     */
    const TypeId type_;
    /**
     * Whether the column is nullable
     */
    bool nullable_;
    /**
     * Distribution of values
     */
    Dist dist_;
    /**
     * Min value of the column
     */
    uint64_t min_;
    /**
     * Max value of the column
     */
    uint64_t max_;
    /**
     * Counter to generate serial data
     */
    uint64_t serial_counter_{0};

    /**
     * Constructor
     */
    ColumnInsertMeta(const char *name, const TypeId type, bool nullable, Dist dist, uint64_t min, uint64_t max)
        : name_(name), type_(type), nullable_(nullable), dist_(dist), min_(min), max_(max) {}
  };

  /**
   * Metadata about a table. Specifically, the schema and number of
   * rows in the table.
   */
  struct TableInsertMeta {
    /**
     * Name of the table
     */
    const char *name_;
    /**
     * Number of rows
     */
    uint32_t num_rows_;
    /**
     * Columns
     */
    std::vector<ColumnInsertMeta> col_meta_;

    /**
     * Constructor
     */
    TableInsertMeta(const char *name, uint32_t num_rows, std::vector<ColumnInsertMeta> col_meta)
        : name_(name), num_rows_(num_rows), col_meta_(col_meta) {}
  };

  std::vector<std::pair<int64_t, RID>> FillTable(TableMetadata *info, TableInsertMeta *table_meta);

  std::vector<Value> MakeValues(ColumnInsertMeta *col_meta, uint32_t count);

  std::vector<Value> GenVarcharValues(ColumnInsertMeta *col_meta, uint32_t count){
    std::vector<Value> values;
    std::string t = "i am xf";
    for(uint32_t i = 0; i < count; i++){
      values.push_back(Value(TypeId::VARCHAR, t));
      col_meta->serial_counter_ += 1;
    }
    return values;
  }

  

  template <typename CppType>
  std::vector<Value> GenNumericValues(ColumnInsertMeta *col_meta, uint32_t count);
  
 private:
  Catalog *catalog_;
  BufferPoolManager *bpm_;
};
}  // namespace bustub
