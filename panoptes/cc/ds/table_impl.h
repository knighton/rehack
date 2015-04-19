#ifndef CC_DS_TABLE_IMPL_H_
#define CC_DS_TABLE_IMPL_H_

#include "table.h"

#include <map>
#include <set>
#include <vector>

#include "cc/base/logging.h"

using std::map;
using std::set;
using std::vector;

template <typename RowKey, typename ColumnKey, typename Value>
bool Table<RowKey, ColumnKey, Value>::Init(
        const vector<RowKey>& row_keys, const vector<ColumnKey>& column_keys) {
    // Check row keys.
    set<RowKey> row_keys_set;
    for (unsigned i = 0; i < row_keys.size(); ++i) {
        if (row_keys_set.find(row_keys[i]) != row_keys_set.end()) {
            ERROR("Table: Repeated row key.\n");
            return false;
        }
        row_keys_set.insert(row_keys[i]);
    }

    // Check column keys.
    set<ColumnKey> column_keys_set;
    for (unsigned i = 0; i < column_keys.size(); ++i) {
        if (column_keys_set.find(column_keys[i]) != column_keys_set.end()) {
            ERROR("Table: Repeated column key.\n");
            return false;
        }
        column_keys_set.insert(column_keys[i]);
    }

    // Set and index row keys.
    row_keys_.resize(row_keys.size());
    row_key2x_.clear();
    for (unsigned i = 0; i < row_keys.size(); ++i) {
        const RowKey& rk = row_keys[i];
        row_keys_[i] = rk;
        row_key2x_[rk] = i;
    }

    // Set and index column keys.
    column_keys_.resize(column_keys.size());
    column_key2x_.clear();
    for (unsigned i = 0; i < column_keys.size(); ++i) {
        const ColumnKey& ck = column_keys[i];
        column_keys_[i] = ck;
        column_key2x_[ck] = i;
    }

    // Clear the values.
    values_.clear();
    return true;
}

template <typename RowKey, typename ColumnKey, typename Value>
bool Table<RowKey, ColumnKey, Value>::InitWithValues(
        const vector<RowKey>& row_keys, const vector<ColumnKey>& column_keys,
        const vector<Value>& values) {
    if (row_keys.size() * column_keys.size() != values.size()) {
        ERROR("Table: Dimensions don't match (%zu rows * %zu columns != %zu "
              "values).\n", row_keys.size(), column_keys.size(), values.size());
        return false;
    }

    if (!Init(row_keys, column_keys)) {
        return false;
    }

    values_.resize(values.size());
    for (unsigned i = 0; i < values.size(); ++i) {
        values_[i] = values[i];
    }
    return true;
}

template <typename RowKey, typename ColumnKey, typename Value>
void Table<RowKey, ColumnKey, Value>::Clear() {
    row_keys_.clear();
    row_key2x_.clear();

    column_keys_.clear();
    column_key2x_.clear();

    values_.clear();
}

template <typename RowKey, typename ColumnKey, typename Value>
bool Table<RowKey, ColumnKey, Value>::Find(
        const RowKey& row_key, const ColumnKey& column_key, size_t* x) const {
    // Get row index.
    typename map<RowKey, size_t>::const_iterator it = row_key2x_.find(row_key);
    if (it == row_key2x_.end()) {
        return false;
    }
    size_t row_index = it->second;

    // Get column index.
    typename map<ColumnKey, size_t>::const_iterator jt =
        column_key2x_.find(column_key);
    if (jt == column_key2x_.end()) {
        return false;
    }
    size_t column_index = jt->second;

    // Get value index.
    size_t index = row_index * column_keys_.size() + column_index;
    if (!(0 <= index && index < values_.size())) {
        return false;
    }

    *x = index;
    return true;
}

template <typename RowKey, typename ColumnKey, typename Value>
bool Table<RowKey, ColumnKey, Value>::Get(
        const RowKey& row_key, const ColumnKey& column_key, Value* v) const {
    size_t x;
    if (!Find(row_key, column_key, &x)) {
        return false;
    }

    *v = values_[x];
    return true;
}

template <typename RowKey, typename ColumnKey, typename Value>
Value* Table<RowKey, ColumnKey, Value>::At(
        size_t row_index, size_t column_index) const {
    size_t x = row_index * column_keys_.size() + column_index;
    assert(0 <= x && x < values_.size());
    return &values_[x];
}

template <typename RowKey, typename ColumnKey, typename Value>
bool Table<RowKey, ColumnKey, Value>::Set(
        const RowKey& row_key, const ColumnKey& column_key, const Value& v) {
    size_t x;
    if (!Find(row_key, column_key, &x)) {
        return false;
    }

    values_[x] = v;
    return true;
}

#endif  // CC_DS_TABLE_IMPL_H_
