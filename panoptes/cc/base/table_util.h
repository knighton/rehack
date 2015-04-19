#ifndef CC_BASE_TABLE_UTIL_H_
#define CC_BASE_TABLE_UTIL_H_

#include <string>

#include "cc/ds/table.h"

using std::string;

#define StringTable Table<string, string, string>

class TableUtil {
  public:
    static bool ParseStringTable(const string& s, StringTable* t);

    template <typename RowKey, typename ColumnKey>
    static bool ParseTable(
        const string& text, const vector<RowKey>& row_keys,
        const vector<string>& row_key_check_strs,
        const vector<ColumnKey>& column_keys,
        const vector<string>& column_key_check_strs,
        Table<RowKey, ColumnKey, string>* t);

    static bool BoolFromTF(const string& s, bool* r);
};

#include "table_util_impl.h"

#endif  // CC_BASE_TABLE_UTIL_H_

