#ifndef CC_DS_GENERALIZING_SUFFIX_TREE_IMPL_H_
#define CC_DS_GENERALIZING_SUFFIX_TREE_IMPL_H_

#include "generalizing_suffix_tree.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "cc/base/logging.h"
#include "cc/base/string.h"

using std::map;
using std::string;
using std::vector;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

template <typename T>
GeneralizingSuffixTreeNode<T>::GeneralizingSuffixTreeNode(
        const string& first_key, bool is_leaf, T value) {
    keys_.emplace_back(first_key);
    is_leaf_ = is_leaf;
    value_ = value;
}

template <typename T>
void GeneralizingSuffixTreeNode<T>::AddKey(const string& key) {
    keys_.emplace_back(key);
}

template <typename T>
void GeneralizingSuffixTreeNode<T>::DumpToString(string* s) const {
    s->clear();
    *s += String::StringPrintf("%zu %d", value_, is_leaf_);
    for (const string& key : keys_) {
        *s += String::StringPrintf(" %s", key.c_str());
    }
}

template <typename T>
void GeneralizingSuffixTree<T>::InitFromDict(const map<string, T>& str2val) {
    // Reverse the strings in order to create keys for suffix lookup.
    vector<string> todos;           // list of reversed string.
    for (typename map<string, T>::const_iterator it = str2val.begin();
              it != str2val.end(); ++it) {
        string reversed = it->first;
        // Carat to distinguish ended lemmas from ones that continue.
        reversed = string(reversed.rbegin(), reversed.rend()) + "^";
        todos.emplace_back(reversed);
    }

    // Loop around until done, splitting nodes to resolve conflicts.
    key2node_.clear();
    unsigned round = 0;
    while (todos.size()) {
        DEBUG("[GeneralizingSuffixTree] Round %u has %zu todos.\n", round,
                    todos.size());
        vector<string> next_todos;
        for (const string& reversed : todos) {
            for (size_t j = 0; j < reversed.size(); ++j) {
                // If the prefix isn't in the tree, add myself here and we're
                // done.
                const string key = reversed.substr(0, j);
                string ltr = reversed;
                ltr = string(ltr.rbegin(), ltr.rend());
                T value = str2val.find(ltr.substr(1))->second;
                if (key2node_.find(key) == key2node_.end()) {
                    bool is_leaf = true;
                    key2node_[key] = GeneralizingSuffixTreeNode<T>(
                        reversed, is_leaf, value);
                    break;
                }

                // It's possible to find a node with the correct value, but that
                // has a child with an even more accurate suffix but wrong
                // value, which would screw up the lookup.  If node is internal,
                // keep branching.
                GeneralizingSuffixTreeNode<T>* node =
                    &(key2node_.find(key)->second);
                if (!node->is_leaf()) {
                    continue;
                }

                // Leaf node: if we have the same value, we're good, but add
                // myself to the keys (may need to split later).
                if (value == node->value()) {
                    node->AddKey(key);
                    break;
                }

                // Else, we disagreed about what the key should be.
                //
                // Pick which value to keep, throw its keys on the next round's
                // todo list, and keep looking with a longer suffix.
                for (const string& s : node->keys()) {
                    next_todos.emplace_back(s);
                }
                T picked_value = MIN(node->value(), value);
                node->clear_keys();
                node->set_is_leaf(false);
                node->set_value(picked_value);
            }
        }

        todos.resize(next_todos.size());
        for (size_t i = 0; i < next_todos.size(); ++i) {
            todos[i] = next_todos[i];
        }

        ++round;
    }

    for (auto& it : key2node_) {
        GeneralizingSuffixTreeNode<T>* node = &it.second;
        if (!node->is_leaf()) {
            node->clear_keys();
        }
    }
}

template <typename T>
void GeneralizingSuffixTree<T>::Get(const string& suffix, T* value) const {
    string reversed = string(suffix.rbegin(), suffix.rend());
    for (size_t i = 0; i < reversed.size(); ++i) {
        // Narrowest scope first.
        const string key = reversed.substr(0, reversed.size() - i);
        const auto& it = key2node_.find(key);
        if (it != key2node_.end()) {
            *value = it->second.value();
            return;
        }
    }
    assert(false);
}

template <typename T>
void GeneralizingSuffixTree<T>::DumpToString(string* s) const {
    s->clear();
    for (typename map<string, GeneralizingSuffixTreeNode<T> >::const_iterator it
         = key2node_.begin(); it != key2node_.end(); ++it) {
        string tmp;
        it->second.DumpToString(&tmp);
        *s += String::StringPrintf("%s = %s\n", it->first.c_str(), tmp.c_str());
    }
}

#endif  // CC_DS_GENERALIZING_SUFFIX_TREE_IMPL_H_
