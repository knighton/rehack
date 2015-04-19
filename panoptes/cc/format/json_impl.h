#ifndef CC_BASE_JSON_IMPL_H_
#define CC_BASE_JSON_IMPL_H_

#include "json.h"

#include <string>
#include <vector>

#include "cc/base/string.h"
#include "cc/base/unicode.h"

using std::string;
using std::to_string;
using std::vector;

using unicode::CodePoint;
using unicode::ReadNextUTF8;
using unicode::ExpectNextUTF8;

namespace json {

// -----------------------------------------------------------------------------
// To JSON.

template <typename T>
void AppendInt(const void* p, string* r) {
    const T& n = *static_cast<const T*>(p);
    *r += to_string(n);
}

template <typename T>
void AppendFloat(const void* p, string* r) {
    const T& f = *static_cast<const T*>(p);
    *r += to_string(f);
}

template <typename T>
void VectorToJSON(const vector<T>& v, Type type, string* s) {
    *s += '[';
    for (size_t i = 0; i < v.size(); ++i) {
        APPEND_TYPE[type](static_cast<const void*>(&v[i]), s);
        if (i < v.size() - 1) {
            *s += ',';
        }
    }
    *s += ']';
}

template <typename Value>
void MapToJSON(const map<string, Value>& m, Type type, string* s) {
    vector<Entry> entries;
    entries.reserve(m.size());
    for (auto& it : m) {
        void* value = static_cast<void*>(const_cast<Value*>(&it.second));
        Entry e(type, it.first, value);
        entries.emplace_back(e);
    }

    string tmp;
    EntriesToJSON(entries, &tmp);
    *s += tmp;
}

template <typename T>
void* VoidPtr(const T& t) {
        const void* p = static_cast<const void*>(&t);
        return const_cast<void*>(p);
}

template <typename V0>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0) {
    vector<Entry> v;
    v.reserve(1);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1) {
    vector<Entry> v;
    v.reserve(2);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2) {
    vector<Entry> v;
    v.reserve(3);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3) {
    vector<Entry> v;
    v.reserve(4);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4) {
    vector<Entry> v;
    v.reserve(5);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4,
        Type type5, const string& key5, const V5& value5) {
    vector<Entry> v;
    v.reserve(6);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    v.emplace_back(Entry(type5, key5, VoidPtr(&value5)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4,
        Type type5, const string& key5, const V5& value5,
        Type type6, const string& key6, const V6& value6) {
    vector<Entry> v;
    v.reserve(7);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    v.emplace_back(Entry(type5, key5, VoidPtr(&value5)));
    v.emplace_back(Entry(type6, key6, VoidPtr(&value6)));
    EntriesToJSON(v, s);
}


template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4,
        Type type5, const string& key5, const V5& value5,
        Type type6, const string& key6, const V6& value6,
        Type type7, const string& key7, const V7& value7) {
    vector<Entry> v;
    v.reserve(8);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    v.emplace_back(Entry(type5, key5, VoidPtr(&value5)));
    v.emplace_back(Entry(type6, key6, VoidPtr(&value6)));
    v.emplace_back(Entry(type7, key7, VoidPtr(&value7)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4,
        Type type5, const string& key5, const V5& value5,
        Type type6, const string& key6, const V6& value6,
        Type type7, const string& key7, const V7& value7,
        Type type8, const string& key8, const V8& value8) {
    vector<Entry> v;
    v.reserve(9);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    v.emplace_back(Entry(type5, key5, VoidPtr(&value5)));
    v.emplace_back(Entry(type6, key6, VoidPtr(&value6)));
    v.emplace_back(Entry(type7, key7, VoidPtr(&value7)));
    v.emplace_back(Entry(type8, key8, VoidPtr(&value8)));
    EntriesToJSON(v, s);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8, typename V9>
void ToJSON(
        string* s,
        Type type0, const string& key0, const V0& value0,
        Type type1, const string& key1, const V1& value1,
        Type type2, const string& key2, const V2& value2,
        Type type3, const string& key3, const V3& value3,
        Type type4, const string& key4, const V4& value4,
        Type type5, const string& key5, const V5& value5,
        Type type6, const string& key6, const V6& value6,
        Type type7, const string& key7, const V7& value7,
        Type type8, const string& key8, const V8& value8,
        Type type9, const string& key9, const V9& value9) {
    vector<Entry> v;
    v.reserve(10);
    v.emplace_back(Entry(type0, key0, VoidPtr(&value0)));
    v.emplace_back(Entry(type1, key1, VoidPtr(&value1)));
    v.emplace_back(Entry(type2, key2, VoidPtr(&value2)));
    v.emplace_back(Entry(type3, key3, VoidPtr(&value3)));
    v.emplace_back(Entry(type4, key4, VoidPtr(&value4)));
    v.emplace_back(Entry(type5, key5, VoidPtr(&value5)));
    v.emplace_back(Entry(type6, key6, VoidPtr(&value6)));
    v.emplace_back(Entry(type7, key7, VoidPtr(&value7)));
    v.emplace_back(Entry(type8, key8, VoidPtr(&value8)));
    v.emplace_back(Entry(type9, key9, VoidPtr(&value9)));
    EntriesToJSON(v, s);
}

// ----------------------------------------------------------------------------
// From JSON.

template <typename T>
bool ParseInt(const string& s, size_t* x, void* p) {
    T n;
    if (!String::ToInt(s, x, &n)) {
        return false;
    }

    *static_cast<T*>(p) = n;
    return true;
}

template <typename T>
bool ParseFloat(const string& s, size_t* x, void* p) {
    T f;
    if (!String::ToFloat(s, x, &f)) {
        return false;
    }

    *static_cast<T*>(p) = f;
    return true;
}

template <typename T>
bool VectorFromJSON(const string& orig_s, Type type, vector<T>* v) {
    v->clear();

    string s;
    RemoveWhitespace(orig_s, &s);

    // Read "[".
    size_t x = 0;
    CodePoint c;
    if (!ExpectNextUTF8(s, &x, &c, '[')) {
        return false;
    }

    while (true) {
        // Parse an entry.
        T t;
        if (!PARSE_TYPE[type](s, &x, &t)) {
            return false;
        }
        v->emplace_back(t);

        if (!ReadNextUTF8(s, &x, &c)) {
            return c == ']';
        }
        if (c == ',') {
            ;
        } else if (c == ']') {
            break;
        } else {
            return false;
        }
    }

    if (!ExpectNextUTF8(s, &x, &c, ']')) {
        return false;
    }

    return true;
}

template <typename Value>
bool MapFromJSON(const string& orig_s, Type type, map<string, Value>* m) {
    m->clear();

    string s;
    RemoveWhitespace(orig_s, &s);

    // Read "{".
    size_t x = 0;
    CodePoint c;
    if (!ExpectNextUTF8(s, &x, &c, '{')) {
        return false;
    }

    while (true) {
        // Parse an entry.
        string key;
        Value value;
        if (!ReadKeyValue(s, type, &x, &key, &value)) {
            return false;
        }
        (*m)[key] = value;

        // Read "," (continue) or "}" (done).
        if (!ReadNextUTF8(s, &x, &c)) {
            return c == '}';
        }
        if (c == '}') {
            break;
        } else if (c == ',') {
            ;
        } else {
            return false;
        }
    }

    return true;
}

template <typename V0>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4,
        Type type5, const string& key5, V5* value5) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    m[key5] = Entry(type5, key5, static_cast<void*>(value5));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4,
        Type type5, const string& key5, V5* value5,
        Type type6, const string& key6, V6* value6) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    m[key5] = Entry(type5, key5, static_cast<void*>(value5));
    m[key6] = Entry(type6, key6, static_cast<void*>(value6));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4,
        Type type5, const string& key5, V5* value5,
        Type type6, const string& key6, V6* value6,
        Type type7, const string& key7, V7* value7) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    m[key5] = Entry(type5, key5, static_cast<void*>(value5));
    m[key6] = Entry(type6, key6, static_cast<void*>(value6));
    m[key7] = Entry(type7, key7, static_cast<void*>(value7));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4,
        Type type5, const string& key5, V5* value5,
        Type type6, const string& key6, V6* value6,
        Type type7, const string& key7, V7* value7,
        Type type8, const string& key8, V8* value8) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    m[key5] = Entry(type5, key5, static_cast<void*>(value5));
    m[key6] = Entry(type6, key6, static_cast<void*>(value6));
    m[key7] = Entry(type7, key7, static_cast<void*>(value7));
    m[key8] = Entry(type8, key8, static_cast<void*>(value8));
    return EntriesFromJSON(s, m);
}

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8, typename V9>
bool FromJSON(
        const string& s,
        Type type0, const string& key0, V0* value0,
        Type type1, const string& key1, V1* value1,
        Type type2, const string& key2, V2* value2,
        Type type3, const string& key3, V3* value3,
        Type type4, const string& key4, V4* value4,
        Type type5, const string& key5, V5* value5,
        Type type6, const string& key6, V6* value6,
        Type type7, const string& key7, V7* value7,
        Type type8, const string& key8, V8* value8,
        Type type9, const string& key9, V9* value9) {
    map<string, Entry> m;
    m[key0] = Entry(type0, key0, static_cast<void*>(value0));
    m[key1] = Entry(type1, key1, static_cast<void*>(value1));
    m[key2] = Entry(type2, key2, static_cast<void*>(value2));
    m[key3] = Entry(type3, key3, static_cast<void*>(value3));
    m[key4] = Entry(type4, key4, static_cast<void*>(value4));
    m[key5] = Entry(type5, key5, static_cast<void*>(value5));
    m[key6] = Entry(type6, key6, static_cast<void*>(value6));
    m[key7] = Entry(type7, key7, static_cast<void*>(value7));
    m[key8] = Entry(type8, key8, static_cast<void*>(value8));
    m[key9] = Entry(type9, key9, static_cast<void*>(value9));
    return EntriesFromJSON(s, m);
}

// -----------------------------------------------------------------------------

}  // namespace json

#endif  // CC_BASE_JSON_IMPL_H_
