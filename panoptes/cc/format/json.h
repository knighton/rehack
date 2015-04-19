#ifndef CC_BASE_JSON_H_
#define CC_BASE_JSON_H_

// JSON conversion.
//
// Usage:
//   -> ToJSON(), VectorToJSON(), MapToJSON()
//   <- ObjectFromJSON(), VectorFromJSON(), MapFromJSON()
//
// For example, see json_test.cpp.

#include <map>
#include <string>
#include <vector>

#include "cc/base/unicode.h"

using std::map;
using std::string;
using std::vector;

using unicode::CodePoint;

namespace json {

enum Type {
    BOOL,
    I8,
    U8,
    I16,
    U16,
    I32,
    U32,
    I64,
    U64,
    SIZET,

    FLOAT,
    DOUBLE,

    CHARS,  // Caller takes ownership of allocated memory.  Will not contain the
                    // full string if it contains \0s.
    STR,

    OBJECT,  // For raw strings (ie, children JSON objects).

    NUM_JSON_TYPES
};

struct Entry {
    Type type;
    string key;
    void* value;

    Entry() : value(NULL) {}

    Entry(Type t, const string& k, void* v) {
        type = t;
        key = k;
        value = v;
    }
};

// -----------------------------------------------------------------------------
// To JSON.

typedef void (*AppendType)(const void* p, string* r);

extern AppendType APPEND_TYPE[NUM_JSON_TYPES];

void AppendBool(const void* p, string* r);

template <typename T>
bool AppendInt(const void* p, string* r);

template <typename T>
bool AppendFloat(const void* p, string* r);

void AppendChars(const void* p, string* r);

void AppendStr(const void* p, string* r);

void AppendObject(const void* p, string* r);

void AppendEntry(const Entry& e, string* r);

void EntriesToJSON(const vector<Entry>& v, string* s);

template <typename T>
void VectorToJSON(const vector<T>& v, Type type, string* s);

template <typename Value>
void MapToJSON(const map<string, Value>& m, Type type, string* s);

void ToJSON(
        string* s);

template <typename V0>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0);

template <typename V0, typename V1>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1);

template <typename V0, typename V1, typename V2>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2);

template <typename V0, typename V1, typename V2, typename V3>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3);

template <typename V0, typename V1, typename V2, typename V3, typename V4>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4,
        Type t5, const string& n5, const V5& v5);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4,
        Type t5, const string& n5, const V5& v5,
        Type t6, const string& n6, const V6& v6);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4,
        Type t5, const string& n5, const V5& v5,
        Type t6, const string& n6, const V6& v6,
        Type t7, const string& n7, const V7& v7);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4,
        Type t5, const string& n5, const V5& v5,
        Type t6, const string& n6, const V6& v6,
        Type t7, const string& n7, const V7& v7,
        Type t8, const string& n8, const V8& v8);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8, typename V9>
void ToJSON(
        string* s,
        Type t0, const string& n0, const V0& v0,
        Type t1, const string& n1, const V1& v1,
        Type t2, const string& n2, const V2& v2,
        Type t3, const string& n3, const V3& v3,
        Type t4, const string& n4, const V4& v4,
        Type t5, const string& n5, const V5& v5,
        Type t6, const string& n6, const V6& v6,
        Type t7, const string& n7, const V7& v7,
        Type t8, const string& n8, const V8& v8,
        Type t9, const string& n9, const V9& v9);

// -----------------------------------------------------------------------------
// From JSON.

typedef bool (*ParseType)(const string& s, size_t* x, void* p);

extern ParseType PARSE_TYPE[NUM_JSON_TYPES];

bool IsFieldEndChar(CodePoint c);

void ParseNonquotedNonescapedField(const string& s, size_t* x, string* field);

bool ParseBool(const string& s, size_t* x, void* p);

template <typename T>
bool ParseInt(const string& s, size_t* x, void* p);

template <typename T>
bool ParseFloat(const string& s, size_t* x, void* p);

bool ParseQuotedAndEscaped(const string& s, size_t* x, string* field);

bool ParseChars(const string& s, size_t* x, void* p);

bool ParseStr(const string& s, size_t* x, void* p);

bool ParseObject(const string& s, size_t* x, void* p);

bool IsWhitespaceCharacter(CodePoint c);

void RemoveWhitespace(const string& in, string* out);

bool ReadKeyValue(const string& s, Type type, size_t* x,
                                    string* key, void* value);

bool EntriesFromJSON(const string& s, const map<string, Entry>& m);

template <typename T>
bool VectorFromJSON(const string& s, Type type, vector<T>* v);

template <typename Value>
bool MapFromJSON(const string& s, Type type, map<string, Value>* m);

bool FromJSON(
        const string& s);

template <typename V0>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0);

template <typename V0, typename V1>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1);

template <typename V0, typename V1, typename V2>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2);

template <typename V0, typename V1, typename V2, typename V3>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3);

template <typename V0, typename V1, typename V2, typename V3, typename V4>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4,
        Type t5, const string& n5, V5* v5);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4,
        Type t5, const string& n5, V5* v5,
        Type t6, const string& n6, V6* v6);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4,
        Type t5, const string& n5, V5* v5,
        Type t6, const string& n6, V6* v6,
        Type t7, const string& n7, V7* v7);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4,
        Type t5, const string& n5, V5* v5,
        Type t6, const string& n6, V6* v6,
        Type t7, const string& n7, V7* v7,
        Type t8, const string& n8, V8* v8);

template <typename V0, typename V1, typename V2, typename V3, typename V4,
                    typename V5, typename V6, typename V7, typename V8, typename V9>
bool FromJSON(
        const string& s,
        Type t0, const string& n0, V0* v0,
        Type t1, const string& n1, V1* v1,
        Type t2, const string& n2, V2* v2,
        Type t3, const string& n3, V3* v3,
        Type t4, const string& n4, V4* v4,
        Type t5, const string& n5, V5* v5,
        Type t6, const string& n6, V6* v6,
        Type t7, const string& n7, V7* v7,
        Type t8, const string& n8, V8* v8,
        Type t9, const string& n9, V9* v9);

// -----------------------------------------------------------------------------

}  // namespace json

#include "json_impl.h"

#endif  // CC_BASE_JSON_H_
