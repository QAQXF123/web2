

#include "../a-include/catalog/schema.h"

#include <sstream>
#include <string>
#include <vector>

namespace bustub {

Schema::Schema(const std::vector<Column> &columns) : tupleIsInlined_(true){
  uint32_t currOffset = 0;
  for (uint32_t index = 0; index < columns.size(); index++) {
    Column column = columns[index];
    column.columnOffset_ = currOffset;
    currOffset += column.GetFixedLength();
    /*if(column.GetType() == TypeId::VARCHAR){
      currOffset += 4;
    }*/
    // add column
    this->columns_.push_back(column);
  }
  // set tuple length
  length_ = currOffset;
  //printf("schema len in schema %d\n", length_);
 // printf("col cnt:%d\n", columns_.size());
}

std::string Schema::ToString() const {
  std::ostringstream os;

  os << "Schema["
     << "NumColumns:" << GetColumnCount() << ", "
     << "IsInlined:" << tupleIsInlined_ << ", "
     << "Length:" << length_ << "]";

  bool first = true;
  os << " :: (";
  for (uint32_t i = 0; i < GetColumnCount(); i++) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << columns_[i].ToString();
  }
  os << ")";

  return os.str();
}

}  // namespace bustub
