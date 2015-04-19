#ifndef CC_BASE_COMBINATORICS_IMPL_H_
#define CC_BASE_COMBINATORICS_IMPL_H_

#include "combinatorics.h"

#include <cassert>
#include <cstddef>
#include <map>
#include <string>

using std::map;
using std::string;

namespace Combinatorics {

template <typename T>
void InclusiveSpansOfSame(const vector<T>& v, vector<InclusiveSpan>* spans) {
    spans->clear();

    if (v.empty()) {
        return;
    }

    T prev_t = v[0];
    T prev_x = 0;
    InclusiveSpan span;
    for (size_t i = 1; i < v.size(); ++i) {
        T t = v[i];
        if (t != prev_t) {
            span.a = prev_x;
            span.z_incl = i - 1;
            spans->emplace_back(span);

            prev_t = t;
            prev_x = i;
        }
    }

    span.a = prev_x;
    span.z_incl = v.size() - 1;
    spans->emplace_back(span);
}

template <typename T>
bool NextChooseOneFromEach(
        const vector<T>& num_options_per_field, vector<T>* values) {
    // If it's empty, we're done.
    if (!num_options_per_field.size()) {
        return false;
    }

    // If any of the fields have no options, we're done.
    for (size_t i = 0; i < num_options_per_field.size(); ++i) {
        if (!num_options_per_field[i]) {
            return false;
        }
    }

    // Initialize the values to return.
    if (values->size() != num_options_per_field.size()) {
        values->resize(num_options_per_field.size());
        return true;
    }

    // Do the increment.
    ++(*values)[0];

    // Carry.
    size_t i = 0;
    while ((*values)[i] == num_options_per_field[i]) {
        (*values)[i] = 0;
        ++i;
        if (i == values->size()) {
            return false;
        }
        ++(*values)[i];
    }

    // Return that we have results.
    return true;
}

template <typename T>
static void StringFromPointerAndSize(const T* t, size_t size, string* r) {
    const char* s = reinterpret_cast<const char*>(t);
    *r = string(s, size);
}

template <typename T>
static void KeyFromTuple(const vector<T>& v, size_t ignore_field_index,
                  string* key) {
    StringFromPointerAndSize(&v[0], v.size(), key);
    (*key)[ignore_field_index] = ~0;
}

template <typename T>
void TupleFromKey(const string& key, vector<T>* v) {
    v->clear();
    v->reserve(key.size() / sizeof(T));
    const T* data = reinterpret_cast<const T*>(&key[0]);
    for (size_t i = 0; i < key.size() / sizeof(T); ++i) {
        v->emplace_back(data[i]);
    }
}

// Collapse (AB 123 DEF) (A1D A2D A3D B1D B1E) =
//
// 0 -> *1D A2D A3D B1E
// 1 -> A*D B1D B1E  <- chosen
// 2 -> A1D A2D A3D B1D B1E
template <typename T>
void CollapseToWildcards(
        const vector<T>& num_options_per_field, vector<vector<T> >* tuples) {
    // Keep shrinking tuples until we can't anymore.
    while (true) {
        // Field to collapse on -> collapsed tuples.  We pick the best one of
        // these each iteration.
        //
        // For each field to collapse on,
        vector<vector<vector<T> > > f2collapsed_tuples;
        f2collapsed_tuples.resize(num_options_per_field.size());
        for (size_t f = 0; f < num_options_per_field.size(); ++f) {
            // Key -> bitset of whether each option is present.
            map<string, vector<bool> > key2is_presents;

            // For each input tuple,
            for (size_t i = 0; i < tuples->size(); ++i) {
                const vector<T>& v = tuples->at(i);

                // If we've already collapsed on this field, skip it.
                if (v[f] == num_options_per_field[f]) {
                    continue;
                }

                // Get the tuple's lookup key.
                string key;
                KeyFromTuple(v, f, &key);

                // Set the present bit for this field value at this key.
                auto it = key2is_presents.find(key);
                const T& field_value = v[f];
                if (it == key2is_presents.end()) {
                    key2is_presents[key] =
                        vector<bool>(num_options_per_field[f], false);
                    key2is_presents[key][field_value] = true;
                } else {
                    it->second[field_value] = true;
                }
            }

            // For each key,
            for (auto it = key2is_presents.begin(); it != key2is_presents.end();
                 ++it) {
                const string& key = it->first;
                const vector<bool>& is_presents = it->second;

                // Get tuple with wildcard at that field index from its lookup
                // key.
                vector<T> v;
                TupleFromKey(key, &v);

                // See if all field values are present.
                bool all_are_present = true;
                for (size_t i = 0; i < is_presents.size(); ++i) {
                    if (!is_presents[i]) {
                        all_are_present = false;
                        break;
                    }
                }

                // If we have all of them, replace tuples with a tuple with a
                // "match all" at that field and we're done.
                if (all_are_present) {
                    v[f] = num_options_per_field[f];
                    f2collapsed_tuples[f].emplace_back(v);
                    continue;
                }

                // If not (normal case), walk the bitset, appending tuples.
                for (size_t i = 0; i < is_presents.size(); ++i) {
                    if (!is_presents[i]) {
                        continue;
                    }
                    v[f] = static_cast<T>(i);
                    f2collapsed_tuples[f].emplace_back(v);
                }
            }
        }

        // Find which field we collapsed on shrunk our tuples the most.
        size_t min_size = ~0ul;
        size_t min_field = ~0ul;
        for (size_t f = 1; f < f2collapsed_tuples.size(); ++f) {
            const vector<vector<T> >& cts = f2collapsed_tuples[f];

            // If not set, continue.
            if (!cts.size()) {
                continue;
            }

            // Initialize.
            if (min_size == ~0ul) {
                min_size = cts.size();
                min_field = f;
            }

            // Take the min.
            if (cts.size() < min_size) {
                min_size = cts.size();
                min_field = f;
            }
        }
        assert(min_size != ~0ul);

        // If nobody could shrink the tuples, we're done.
        if (min_size == tuples->size()) {
            break;
        }

        // Rewrite the tuples to the shrunken tuples.
        tuples->clear();
        tuples->reserve(f2collapsed_tuples[min_field].size());
        for (size_t i = 0; i < f2collapsed_tuples[min_field].size(); ++i) {
            tuples->emplace_back(f2collapsed_tuples[min_field][i]);
        }
    }
}

}  // namespace

#endif  // CC_BASE_COMBINATORICS_IMPL_H_
