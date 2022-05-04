#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include "../type/type.h"

namespace bustub{
class AbstractExpression;
class Column{
    friend class Schema;
    public:
    Column(std::string columnName, TypeId type, const AbstractExpression *expr = nullptr)
        : columnName_(std::move(columnName)), columnType_(type), fixedLength_(TypeSize(type)), expr_(expr ){

    }

    Column(std::string columnName, TypeId type, uint32_t length, const AbstractExpression *expr = nullptr)
        : columnName_(std::move(columnName)), columnType_(type), fixedLength_(length), expr_(expr) {

    }

    std::string GetName() const { return columnName_; }


    uint32_t GetLength() const {
        if (IsInlined()) {
            return fixedLength_;
        }
        return variableLength_;
    }
    uint32_t GetFixedLength() const { return fixedLength_; }
    uint32_t GetVariableLength() const { return variableLength_; }
    uint32_t GetOffset() const { return columnOffset_; }

    TypeId GetType() const { return columnType_; }
    bool IsInlined() const { return columnType_ != TypeId::VARCHAR; }
    std::string ToString() const;
    const AbstractExpression *GetExpr() const { return expr_; }



    private:

    static uint8_t TypeSize(TypeId type){
        switch (type){
            case TypeId::BOOLEAN:
                return 1;
            case TypeId::TINYINT:
                return 1;
            //case TypeId::SMALLINT:
              //  return 2;
            case TypeId::INTEGER:
                return 4;
            case TypeId::BIGINT:
            case TypeId::DECIMAL:
            case TypeId::TIMESTAMP:
                return 8;
            case TypeId::VARCHAR:
        // TODO(Amadou): Confirm this.
                return 12;
            default:
                return 0;
        }
    }

    std::string columnName_;
    TypeId columnType_;
    uint32_t fixedLength_;
    uint32_t variableLength_{0};
    uint32_t columnOffset_{0};
    const AbstractExpression *expr_;


};

}