#include "table_util.h"

#include "cc/base/string.h"
#include "cc/ds/table.h"

bool TableUtil::ParseStringTable(const string& s, StringTable* t) {
    // Get lines.
    string trimmed = string(s);
    trimmed = String::Trim(trimmed);
    vector<string> v;
    String::Split(trimmed, '\n', &v);

    // Load the table, row by row.
    vector<vector<string> > vv;
    for (unsigned i = 0; i < v.size(); ++i) {
        vector<string> tmp;
        String::SplitByWhitespace(v[i], &tmp);
        vv.emplace_back(tmp);
    }

    // Verify we have a table.
    if (vv.size() < 2) {
        return false;
    }

    // Verify its dimensions.
    size_t num_columns = vv[0].size();
    for (unsigned i = 1; i < vv.size(); ++i) {
        if (vv[i].size() != num_columns + 1) {
            return false;
        }
    }

    // Extract row keys.
    vector<string> row_keys;
    for (unsigned i = 1; i < vv.size(); ++i) {
        row_keys.emplace_back(vv[i][0]);
    }

    // Extract column keys.
    vector<string> column_keys;
    for (unsigned i = 0; i < vv[0].size(); ++i) {
        column_keys.emplace_back(vv[0][i]);
    }

    // Extract values.
    vector<string> values;
    for (unsigned i = 1; i < vv.size(); ++i) {
        for (unsigned j = 1; j < vv[i].size(); ++j) {
            values.emplace_back(vv[i][j]);
        }
    }

    return t->InitWithValues(row_keys, column_keys, values);
}

bool TableUtil::BoolFromTF(const string& s, bool* r) {
    if (s == "T") {
        *r = true;
    } else if (s == "F") {
        *r = false;
    } else {
        return false;
    }
    return true;
}
