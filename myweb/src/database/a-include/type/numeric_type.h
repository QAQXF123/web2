#pragma once

#include<cmath>
#include "value.h"

namespace bustub{
    class NumericType : public Type{
        public:
            explicit NumericType(TypeId type) : Type(type){}
            ~NumericType() override = default;

            Value Add(const Value &left, const Value &right) const override = 0;
            Value Subtract(const Value &left, const Value &right) const override = 0;
            Value Multiply(const Value &left, const Value &right) const override = 0;
            Value Divide(const Value &left, const Value &right) const override = 0;
            Value Modulo(const Value &left, const Value &right) const override = 0;
            Value Min(const Value &left, const Value &right) const override = 0;
            Value Max(const Value &left, const Value &right) const override = 0;
            Value Sqrt(const Value &val) const override = 0;
            Value OperateNull(const Value &left, const Value &right) const override = 0;
            bool IsZero(const Value &val) const override = 0;

        protected:
            static inline double ValMod(double x, double y){
                return x - std::trunc(static_cast<double>(x) / static_cast<double>(y)) * y;
            }
    };
}
// undefined reference to `bustub::Type::GetData(bustub::Value const&) const'