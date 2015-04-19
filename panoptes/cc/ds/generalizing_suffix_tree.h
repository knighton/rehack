#ifndef CC_DS_GENERALIZING_SUFFIX_TREE_H_
#define CC_DS_GENERALIZING_SUFFIX_TREE_H_

#include <cassert>
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

template <typename T>
class GeneralizingSuffixTreeNode {
  public:
    GeneralizingSuffixTreeNode() { }
    GeneralizingSuffixTreeNode(const string& first_key, bool is_leaf, T value);

    const vector<string>& keys() const { return keys_; }
    void AddKey(const string& key);
    void clear_keys() { keys_.clear(); }

    bool is_leaf() const { return is_leaf_; }
    void set_is_leaf(bool is_leaf) { is_leaf_ = is_leaf; }

    T value() const { return value_; }
    void set_value(T value) { value_ = value; }

    void DumpToString(string* s) const;

  private:
    vector<string> keys_;
    bool is_leaf_;
    T value_;
};

template <typename T>
class GeneralizingSuffixTree {
  public:
    void InitFromDict(const map<string, T>& str2val);

    void Get(const string& suffix, T* value) const;

    void DumpToString(string* s) const;

  private:
    // substring of reversed string -> GeneralizingSuffixTreeNode.
    map<string, GeneralizingSuffixTreeNode<T> > key2node_;
};

#include "generalizing_suffix_tree_impl.h"

#endif  // CC_DS_GENERALIZING_SUFFIX_TREE_H_
