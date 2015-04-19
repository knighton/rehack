#ifndef CC_DS_TABLE_H_
#define CC_DS_TABLE_H_

#include <map>
#include <vector>

using std::map;
using std::vector;

template <typename RowKey, typename ColumnKey, typename Value>
class Table {
  public:
    const vector<RowKey>& row_keys() const { return row_keys_; }
    const map<RowKey, size_t>& row_key2x() const { return row_key2x_; }
    const vector<ColumnKey>& column_keys() const { return column_keys_; }
    const map<RowKey, size_t>& column_key2x() const { return column_key2x_; }
    const vector<Value>& values() const { return values_; }

    // On invalid input, returns false and doesn't modify anything.
    bool Init(const vector<RowKey>& row_keys,
                        const vector<ColumnKey>& column_keys);
    bool InitWithValues(const vector<RowKey>& row_keys,
                                            const vector<ColumnKey>& column_keys,
                                            const vector<Value>& values);

    void Clear();

    bool Get(const RowKey& row_key, const ColumnKey& column_key, Value* v) const;
    bool Set(const RowKey& row_key, const ColumnKey& column_key, const Value& v);
    Value* At(size_t row_index, size_t column_index) const;

  private:
    bool Find(const RowKey& row_key, const ColumnKey& column_key, size_t* x) const;

    vector<RowKey> row_keys_;
    map<RowKey, size_t> row_key2x_;

    vector<ColumnKey> column_keys_;
    map<ColumnKey, size_t> column_key2x_;

    vector<Value> values_;
};

#include "table_impl.h"

#endif  // CC_DS_TABLE_H_
