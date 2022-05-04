#pragma once

#include <memory>
#include <string>
#include <vector>

#include "column.h"
#include "../type/type.h"

namespace bustub{
class Schema{
public:
    explicit Schema(const std::vector<Column> &columns);
    static Schema *CopySchema(const Schema *from, const std::vector<uint32_t> &attrs){
        std::vector<Column> cols;
        cols.reserve(attrs.size());
        for (const auto i : attrs) {
            cols.emplace_back(from->columns_[i]);
        }
        return new Schema{cols};
    }
    const std::vector<Column> &GetColumns() const { return columns_; }
    const Column &GetColumn(const uint32_t col_idx) const { return columns_[col_idx]; }

    uint32_t GetColIdx(const std::string &col_name) const {
        for (uint32_t i = 0; i < columns_.size(); ++i) {
            if (columns_[i].GetName() == col_name) {
                return i;
            }
        }
        printf("fail to getCloIdx\n");
        return -1;
    }

    const std::vector<uint32_t> &GetUnlinedColumns() const { return uninlinedColumns_; }
    uint32_t cnt() const {return columns_.size(); }
    uint32_t GetColumnCount() const { return static_cast<uint32_t>(columns_.size()); }
    uint32_t GetUnlinedColumnCount() const { return static_cast<uint32_t>(uninlinedColumns_.size()); }
    inline uint32_t GetLength() const { return length_; }
    inline bool IsInlined() const { return tupleIsInlined_; }
    std::string ToString() const;

private:
    uint32_t length_;
    std::vector<Column> columns_;
    bool tupleIsInlined_;
    std::vector<uint32_t> uninlinedColumns_;

};

}