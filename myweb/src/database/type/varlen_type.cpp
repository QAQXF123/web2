//===----------------------------------------------------------------------===//


#include <algorithm>
#include <string>
#include <assert.h>

#include "../a-include/type/varlen_type.h"

namespace bustub {

VarlenType::VarlenType(TypeId type) : Type(type) {}

VarlenType::~VarlenType() = default;

// Access the raw variable length data
const char *VarlenType::GetData(const Value &val) const { return val.value_.varlen_; }

// Get the length of the variable length data (including the length field)
uint32_t VarlenType::GetLength(const Value &val) const { return val.size_.len_; }




std::string VarlenType::ToString(const Value &val) const {
  uint32_t len = GetLength(val);

  if (val.IsNull()) {
    return "varlen_null";
  }
  if (len == BUSTUB_VARCHAR_MAX_LEN) {
    return "varlen_max";
  }
  if (len == 0) {
    return "";
  }
  return std::string(GetData(val), len - 1);
}

void VarlenType::SerializeTo(const Value &val, char *storage) const {
  uint32_t len = GetLength(val);
 // printf("len of str %d\n", len);
  if (len == BUSTUB_VALUE_NULL) {
    memcpy(storage, &len, sizeof(uint32_t));
    return;
  }
  memcpy(storage, &len, sizeof(uint32_t));
  memcpy(storage + sizeof(uint32_t), val.value_.varlen_, len);
}

// Deserialize a value of the given type from the given storage space.
Value VarlenType::DeserializeFrom(const char *storage) const {
  uint32_t len = *reinterpret_cast<const uint32_t *>(storage);
 // printf("aaaaaaalen %d\n", len);
  if (len == BUSTUB_VALUE_NULL) {
    return Value(typeId_, nullptr, len, false);
  }
  // set manage_data as true
  return Value(typeId_, storage + sizeof(uint32_t), len, true);
}

Value VarlenType::Copy(const Value &val) const { return Value(val); }


    

}  // namespace bustub
