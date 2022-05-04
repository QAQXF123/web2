#include "../../a-include/catalog/table_generator.h"

#include <algorithm>
#include <random>
#include <vector>

namespace bustub {


template <typename CppType>
std::vector<Value> TableGenerator::GenNumericValues(ColumnInsertMeta *col_meta, uint32_t count) {

 std::vector<Value> values;
 auto type = col_meta->type_;
  
  for (uint32_t i = 0; i < count; i++) {
    values.push_back(Value(type, static_cast<CppType>(col_meta->serial_counter_)));
    col_meta->serial_counter_ += 1;
  }
    
  return values;
  
  
}

std::vector<Value> TableGenerator::MakeValues(ColumnInsertMeta *col_meta, uint32_t count) {
  switch (col_meta->type_) {
    case TypeId::TINYINT:
      return GenNumericValues<int8_t>(col_meta, count);
    case TypeId::SMALLINT:
      return GenNumericValues<int16_t>(col_meta, count);
    case TypeId::INTEGER:
      return GenNumericValues<int32_t>(col_meta, count);
    case TypeId::BIGINT:
      return GenNumericValues<int64_t>(col_meta, count);
    case TypeId::DECIMAL:
      return GenNumericValues<double>(col_meta, count);
    case TypeId::VARCHAR:
      return GenVarcharValues(col_meta, count);
    default:
      return GenNumericValues<double>(col_meta, count);
      //UNREACHABLE("Not yet implemented");
  }
}

std::vector<std::pair<int64_t, RID>> TableGenerator::FillTable(TableMetadata *info, TableInsertMeta *table_meta) {
  uint32_t num_inserted = 0;
  uint32_t batch_size = 128;
  std::vector<std::pair<int64_t, RID>> res{};
  while (num_inserted < table_meta->num_rows_) {
    std::vector<std::vector<Value>> values;
    uint32_t num_values = std::min(batch_size, table_meta->num_rows_ - num_inserted);
    for (auto &col_meta : table_meta->col_meta_) {
      values.push_back(MakeValues(&col_meta, num_values));
    }
    for (uint32_t i = 0; i < num_values; i++) {
      std::vector<Value> entry;
      entry.reserve(values.size());
      for (const auto &col : values) {
        entry.push_back(col[i]);
      }
      RID rid;
      
      Tuple tuple(entry, &info->schema_);
      printf("tuple size in filltable:%d\n", tuple.GetLength());
      //bool inserted = info->table_->InsertTuple(Tuple(entry, &info->schema_), &rid);
      bool inserted = info->table_->InsertTuple(tuple, &rid);
    
      printf("rid pageid:%d, slotid:%d\n ", rid.GetPageId(), rid.GetSlotNum());
      assert(inserted == true);
      res.push_back({entry[0].GetBigInt(entry[0]), rid});
      //BUSTUB_ASSERT(inserted, "Sequential insertion cannot fail");
      num_inserted++;
    }
    GetBufferPoolManager()->FlushAllPages();
  }
  return res;
  //LOG_INFO("Wrote %d tuples to table %s.", num_inserted, table_meta->name_);
}

 std::vector<std::pair<int64_t, RID>> TableGenerator::GenerateTestTables() {
  /**
   * This array configures each of the test tables. Each table is configured
   * with a name, size, and schema. We also configure the columns of the table. If
   * you add a new table, set it up here.
   */
  /*std::vector<TableInsertMeta> insert_meta{
      {"test_1",
        10, //TEST1_SIZE,
       {{"key", TypeId::INTEGER, false, Dist::Serial, 0, 0},
        {"name", TypeId::VARCHAR, false, Dist::Uniform, 0, 9},
        {"colB", TypeId::INTEGER, false, Dist::Uniform, 0, 9},
        {"colC", TypeId::INTEGER, false, Dist::Uniform, 0, 9999},
        {"colD", TypeId::INTEGER, false, Dist::Uniform, 0, 99999}}},
     
  };

  for (auto &table_meta : insert_meta) {
    // Create Schema
    std::vector<Column> cols{};
    cols.reserve(table_meta.col_meta_.size());
   // printf("cols.cap %d\n", cols.capacity());
    for (const auto &col_meta : table_meta.col_meta_) {
      if (col_meta.type_ != TypeId::VARCHAR) {
        cols.push_back({col_meta.name_, col_meta.type_});
      } else {
        cols.push_back({col_meta.name_, col_meta.type_, 20});
      }
    }
    //printf("cols.size %d\n", cols.size());
    Schema schema(cols);
    //printf("col cnt %d\n", schema.cnt());
    auto info = GetCatalog()->CreateTable(table_meta.name_, schema);
    printf("cnt of info schema %d\n", info->schema_.cnt());
    return FillTable(info, &table_meta);
  }*/
  return {};
}
}
