#include <cassert>
#include <string>
#include <utility>

#include "../a-include/type/value.h"

namespace bustub{
    Value::Value(const Value &other){
        typeId_ = other.typeId_;
        size_ = other.size_;
        manageData_ = other.manageData_;
        value_ = other.value_;
        switch (typeId_){
            case TypeId::VARCHAR:
                if (size_.len_ == BUSTUB_VALUE_NULL){
                    value_.varlen_ = nullptr;
                } else{
                    if(manageData_){
                        value_.varlen_ = new char[size_.len_];
                        memcpy(value_.varlen_, other.value_.varlen_, size_.len_);
                    }else{
                        value_ = other.value_;
                    }
                }
                break;
            default:
                value_ = other.value_;
        }
    }

    Value &Value::operator=(Value other) {
        Swap(*this, other);
        return *this;
    }

    Value::Value(TypeId type, int8_t i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }
    Value::Value(TypeId type, int16_t i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }

    Value::Value(TypeId type, int32_t i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }
    Value::Value(TypeId type, int64_t i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }
        Value::Value(TypeId type, unsigned long i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }
    Value::Value(TypeId type, unsigned long long i) : Value(type) {
        switch (type) {
            case TypeId::BOOLEAN:
                value_.boolean_ = i;
                size_.len_ = (value_.boolean_ == BUSTUB_BOOLEAN_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::TINYINT:
                value_.tinyint_ = i;
                size_.len_ = (value_.tinyint_ == BUSTUB_INT8_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            /*case TypeId::SMALLINT:
                value_.smallint_ = i;
                size_.len_ = (value_.smallint_ == BUSTUB_INT16_NULL ? BUSTUB_VALUE_NULL : 0);
                break;*/
            case TypeId::INTEGER:
                value_.integer_ = i;
                size_.len_ = (value_.integer_ == BUSTUB_INT32_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            case TypeId::BIGINT:
                value_.bigint_ = i;
                size_.len_ = (value_.bigint_ == BUSTUB_INT64_NULL ? BUSTUB_VALUE_NULL : 0);
            break;
            default:
                printf("error type\n");
                //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type for one-byte Value constructor");
        }
    }

    //float
    Value::Value(TypeId type, double d) : Value(type) {
        switch (type) {
            case TypeId::DECIMAL:
                value_.decimal_ = d;
                size_.len_ = (value_.decimal_ == BUSTUB_DECIMAL_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            default:
                printf("error type\n");

        }
    }

    Value::Value(TypeId type, float f) : Value(type) {
        switch (type) {
            case TypeId::DECIMAL:
                value_.decimal_ = f;
                size_.len_ = (value_.decimal_ == BUSTUB_DECIMAL_NULL ? BUSTUB_VALUE_NULL : 0);
                break;
            default:
                printf("error type\n");

        }
    }
    //varchar
    Value::Value(TypeId type, const char *data, uint32_t len, bool manage_data) : Value(type) {
        switch (type) {
            case TypeId::VARCHAR:
                if (data == nullptr) {
                    value_.varlen_ = nullptr;
                    size_.len_ = BUSTUB_VALUE_NULL;
                } else {
                    manageData_ = manage_data;
                    if (manageData_) {
                        assert(len < BUSTUB_VARCHAR_MAX_LEN);
                        value_.varlen_ = new char[len];
                        assert(value_.varlen_ != nullptr);
                        size_.len_ = len;
                        memcpy(value_.varlen_, data, len);
                    } else {
                    // FUCK YOU GCC I do what I want.
                        value_.const_varlen_ = data;
                        size_.len_ = len;
                    }
                }
                break;
            default:
                printf("error type\n");
                
        }
    }


Value::Value(TypeId type, const std::string &data) : Value(type) {
  switch (type) {
    case TypeId::VARCHAR: {
      manageData_ = true;
      // TODO(TAs): How to represent a null string here?
      uint32_t len = static_cast<uint32_t>(data.length()) + 1;
      value_.varlen_ = new char[len];
      assert(value_.varlen_ != nullptr);
      size_.len_ = len;
      memcpy(value_.varlen_, data.c_str(), len);
      break;
    }
    default:
        printf("error type\n");
      //throw Exception(ExceptionType::INCOMPATIBLE_TYPE, "Invalid Type  for variable-length Value constructor");
  }
}

Value::~Value() {
  switch (typeId_) {
    case TypeId::VARCHAR:
      if (manageData_) {
        delete[] value_.varlen_;
      }
      break;
    default:
      break;
  }
}

bool Value::CheckComparable(const Value &o) const {
  switch (GetTypeId()) {
    case TypeId::BOOLEAN:
      return (o.GetTypeId() == TypeId::BOOLEAN || o.GetTypeId() == TypeId::VARCHAR);
    case TypeId::TINYINT:
   // case TypeId::SMALLINT:
    case TypeId::INTEGER:
    case TypeId::BIGINT:
    case TypeId::DECIMAL:
      switch (o.GetTypeId()) {
        case TypeId::TINYINT:
      //  case TypeId::SMALLINT:
        case TypeId::INTEGER:
        case TypeId::BIGINT:
        case TypeId::DECIMAL:
        case TypeId::VARCHAR:
          return true;
        default:
          break;
      }  // SWITCH
      break;
    case TypeId::VARCHAR:
      // Anything can be cast to a string!
      return true;
      break;
    default:
      break;
  }  // END OF SWITCH
  return false;
}


bool Value::CheckInteger() const {
  switch (GetTypeId()) {
    case TypeId::TINYINT:
   // case TypeId::SMALLINT:
    case TypeId::INTEGER:
    case TypeId::BIGINT:
      return true;
    default:
      break;
  }
  return false;
   
}

}