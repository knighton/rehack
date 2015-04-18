#ifndef CC_BASE_ENUM_STRINGS_IMPL_H_
#define CC_BASE_ENUM_STRINGS_IMPL_H_

#include "cc/base/string.h"

template <typename E>
vector<E> EnumRange(uint64_t a, uint64_t z_excl) {
    vector<E> v;
    for (auto i = a; i < z_excl; ++i) {
        E e = static_cast<E>(i);
        v.emplace_back(e);
    }
    return v;
}

template <typename E>
EnumStrings<E>::EnumStrings(const string& text) {
    vector<string> v;
    String::Split(text, ' ', &v);

    enum_values_ = EnumRange<E>(0, v.size());
    strings_.reserve(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        const string& s = v[i];
        string2enum_value_[s] = static_cast<E>(i);
        strings_.emplace_back(s);
    }

    enum_values_except_last_ = EnumRange<E>(0, v.size() - 1);
    strings_except_last_.reserve(v.size() - 1);
    for (size_t i = 0; i < v.size() - 1; ++i) {
        const string& s = v[i];
        string_except_last2enum_value_[s] = static_cast<E>(i);
        strings_except_last_.emplace_back(s);
    }
}

template <typename E>
const string& EnumStrings<E>::GetString(E e) const {
    return strings_[e];
}

template <typename E>
bool EnumStrings<E>::MaybeGetString(E e, string* s) const {
    if (e < 0 || strings_.size() <= e) {
        return false;
    }

    *s = strings_[e];
    return true;
}

template <typename E>
E EnumStrings<E>::GetEnumValue(const string& s) const {
    auto it = string2enum_value_.find(s);
    return it->second;
}

template <typename E>
bool EnumStrings<E>::MaybeGetEnumValue(const string& s, E* e) const {
    auto it = string2enum_value_.find(s);
    if (it == string2enum_value_.end()) {
        return false;
    }

    *e = it->second;
    return true;
}

#endif  // CC_BASE_ENUM_STRINGS_IMPL_H_
