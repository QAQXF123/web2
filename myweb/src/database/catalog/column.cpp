
#include "../a-include/catalog/column.h"
#include "../a-include/type/type.h"

#include <sstream>
#include <string>

namespace bustub {

std::string Column::ToString() const {
  std::ostringstream os;

  /*os << "Column[" << columnName_ << ", " << Type::TypeIdToString(columnType_) << ", "
     << "Offset:" << columnOffset_ << ", ";

  if (IsInlined()) {
    os << "FixedLength:" << fixedLength_;
  } else {
    os << "VarLength:" << variableLength_;
  }
  os << "]";*/
  return (os.str());
}

}  // namespace bustub
