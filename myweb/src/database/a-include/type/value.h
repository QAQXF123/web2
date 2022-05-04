#pragma once

#include<cstring>
#include<string>
#include "limits.h"
#include "type.h"

namespace bustub{
    inline CmpBool GetCmpBool(bool boolean) { return boolean ? CmpBool::CmpTrue : CmpBool::CmpFalse; }
    class Value{
        friend class Type;
        friend class NumericType;
        friend class IntegerParentType;
        friend class IntegerType;
        friend class BigintType;
        friend class DecimalType;
        friend class TimestampType;
        friend class BooleanType;
        friend class VarlenType;

        public:
            explicit Value(const TypeId type) 
            : manageData_(false), typeId_(type) { size_.len_ = BUSTUB_VALUE_NULL; }
            Value(TypeId type, int8_t i);
            Value(TypeId type, double d);
            Value(TypeId type, float f);
            Value(TypeId type, int16_t i);
            Value(TypeId type, int32_t i);
            Value(TypeId type, int64_t i);
          //  Value(TypeId type, uint64_t i);
            Value(TypeId type, unsigned long long i);
            Value(TypeId type, unsigned long i);
            Value(TypeId type, const char *data, uint32_t len, bool manage_data);
            Value(TypeId type, const std::string &data);
            Value() : Value(TypeId::INVALID) {}
            Value(const Value &other);
            Value &operator=(Value other);
            ~Value();

            friend void Swap(Value &first, Value &second) {
                std::swap(first.value_, second.value_);
                std::swap(first.size_, second.size_);
                std::swap(first.manageData_, second.manageData_);
                std::swap(first.typeId_, second.typeId_);
            }
            
            bool CheckInteger() const;
            bool CheckComparable(const Value &o) const;
            inline TypeId GetTypeId() const { return typeId_; }
            inline uint32_t GetLength() const { return Type::GetInstance(typeId_)->GetLength(*this); }
            inline const char *GetData() const { return Type::GetInstance(typeId_)->GetData(*this); }


            template <class T>
            inline T GetAs() const {
                return *reinterpret_cast<const T *>(&value_);
            }
            inline Value CastAs(const TypeId type_id) const {
                return Type::GetInstance(typeId_)->CastAs(*this, type_id); 
            }
            inline CmpBool CompareEquals(const Value &o) const {
                return Type::GetInstance(typeId_)->CompareEquals(*this, o); 
            }
            inline CmpBool CompareNotEquals(const Value &o) const {
                return Type::GetInstance(typeId_)->CompareNotEquals(*this, o);
            }
            inline CmpBool CompareLessThan(const Value &o) const {
                //``1std::cout<<"type--"<<Type::GetInstance(typeId_)<<"\n";
                return Type::GetInstance(typeId_)->CompareLessThan(*this, o);
            }
            inline CmpBool CompareLessThanEquals(const Value &o) const {
                return Type::GetInstance(typeId_)->CompareLessThanEquals(*this, o);
            }
            inline CmpBool CompareGreaterThan(const Value &o) const {
                return Type::GetInstance(typeId_)->CompareGreaterThan(*this, o);
            }
            inline CmpBool CompareGreaterThanEquals(const Value &o) const {
                return Type::GetInstance(typeId_)->CompareGreaterThanEquals(*this, o);
            }


            inline Value Add(const Value &o) const{ return Type::GetInstance(typeId_)->Add(*this, o); }
            inline Value Subtract(const Value &o) const { return Type::GetInstance(typeId_)->Subtract(*this, o); }
            inline Value Multiply(const Value &o) const { return Type::GetInstance(typeId_)->Multiply(*this, o); }
            inline Value Divide(const Value &o)const { return Type::GetInstance(typeId_)->Divide(*this, o); }
            inline Value Modulo(const Value &o)const { return Type::GetInstance(typeId_)->Modulo(*this, o); }
            inline Value Min(const Value &o)const { return Type::GetInstance(typeId_)->Min(*this, o); }
            inline Value Max(const Value &o)const { return Type::GetInstance(typeId_)->Max(*this, o); }
            inline Value Sqrt() const { return Type::GetInstance(typeId_)->Sqrt(*this); }


            inline Value OperateNull(const Value &o) const { return Type::GetInstance(typeId_)->OperateNull(*this, o); }
            inline bool IsZero() const { return Type::GetInstance(typeId_)->IsZero(*this); }
            inline bool IsNull() const { return size_.len_ == BUSTUB_VALUE_NULL; }


            inline void SerializeTo(char *storage) const { Type::GetInstance(typeId_)->SerializeTo(*this, storage); }

            inline static Value DeserializeFrom(const char *storage, const TypeId typeId) {
                return Type::GetInstance(typeId)->DeserializeFrom(storage);
            }
            static int64_t GetBigInt(const Value &value){
                return value.value_.bigint_;

            }
            static char* GetCString(const Value &value){
                return value.value_.varlen_;
            }
            inline std::string ToString() const { return Type::GetInstance(typeId_)->ToString(*this); }
            inline Value Copy() const { return Type::GetInstance(typeId_)->Copy(*this); }

        protected:
            union Val{
                int8_t boolean_;
                int8_t tinyint_;
                int16_t smallint_;
                int32_t integer_;
                int64_t bigint_;
                double decimal_;
                uint64_t timestamp_;
                char *varlen_;
                const char *const_varlen_;
            } value_;

            union {
                uint32_t len_;
                TypeId elemTypeId_;
            } size_;
            
            bool manageData_;
            TypeId typeId_;
            
    };


}
