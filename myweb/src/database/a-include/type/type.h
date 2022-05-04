#pragma once
#include"cstdint"
#include"string"
#include"type_id.h"

namespace bustub{
    class Value;
    enum class CmpBool{ CmpFalse = 0, CmpTrue = 1, CmpNull = 2};
    class Type{
        public:
            explicit Type(TypeId typeId) : typeId_(typeId){}
            virtual ~Type() = default;
            static uint64_t GetTypeSize(TypeId typeID);
            bool IsCorecableFrom(TypeId typeId) const;

            static Value GetMinValue(TypeId typeId);
            static Value GetMaxValue(TypeId typeId);
            inline static Type *GetInstance(TypeId type_id) { return k_types[type_id]; }
            inline TypeId GetTypeId() const { return typeId_; }

            virtual CmpBool CompareEquals(const Value &left, const Value &right) const;
            virtual CmpBool CompareNotEquals(const Value &left, const Value &right) const;
            virtual CmpBool CompareLessThan(const Value &left, const Value &right) const;
            virtual CmpBool CompareLessThanEquals(const Value &left, const Value &right) const;
            virtual CmpBool CompareGreaterThan(const Value &left, const Value &right) const;
            virtual CmpBool CompareGreaterThanEquals(const Value &left, const Value &right) const;

            virtual Value Add(const Value &left, const Value &right) const;
            virtual Value Subtract(const Value &left, const Value &right) const;
            virtual Value Multiply(const Value &left, const Value &right)const;
            virtual Value Divide(const Value &left, const Value &right) const;
            virtual Value Modulo(const Value &left, const Value &right)const;
            virtual Value Min(const Value &left, const Value &right) const;
            virtual Value Max(const Value &left, const Value &right) const;
            virtual Value Sqrt(const Value &val) const;
            virtual Value OperateNull(const Value &val, const Value &right) const;

            
            virtual bool IsZero(const Value &val) const;
            virtual bool IsInlined(const Value &val) const;
            virtual std::string ToString(const Value &val) const;
            virtual void SerializeTo(const Value &val, char *storage) const;
            virtual Value DeserializeFrom(const char *storage) const;
            virtual Value Copy(const Value &val) const;
            virtual Value CastAs(const Value &val, TypeId type_id) const;
            virtual const char *GetData(const Value &val) const;
            virtual uint32_t GetLength(const Value &val) const;
            virtual char *GetData(char *storage)const;
            
        protected:
            TypeId typeId_;
            static Type *k_types[14];
    };
}
