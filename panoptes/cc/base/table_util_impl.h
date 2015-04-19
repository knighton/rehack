#ifndef CC_BASE_TABLE_UTIL_IMPL_H_
#define CC_BASE_TABLE_UTIL_IMPL_H_

#include "cc/base/logging.h"

template <typename RowKey, typename ColumnKey>
bool TableUtil::ParseTable(
        const string& text, const vector<RowKey>& row_keys,
        const vector<string>& row_key_check_strs,
        const vector<ColumnKey>& column_keys,
        const vector<string>& column_key_check_strs,
        Table<RowKey, ColumnKey, string>* t) {
    Table<string, string, string> st;
    if (!TableUtil::ParseStringTable(text, &st)) {
        ERROR("TableUtil: Could not parse the table as strings.\n");
        return false;
    }
    if (st.row_keys() != row_key_check_strs) {
        ERROR("TableUtil: Row keys don't match (sizes: expected %zu, got %zu).\n",
                    row_key_check_strs.size(), st.row_keys().size());
        return false;
    }
    if (st.column_keys() != column_key_check_strs) {
        ERROR("TableUtil: Column keys don't match (sizes: expected %zu, got "
                    "%zu).\n", column_key_check_strs.size(), st.column_keys().size());
        return false;
    }

    return t->InitWithValues(row_keys, column_keys, st.values());
}

#endif  // CC_BASE_TABLE_UTIL_IMPL_H_
