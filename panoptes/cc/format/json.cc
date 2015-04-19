#include "json.h"

#include <cassert>
#include <string>
#include <vector>

#include "cc/base/logging.h"
#include "cc/base/string.h"
#include "cc/base/unicode.h"

using std::string;
using std::vector;

using unicode::AppendUTF8;
using unicode::CodePoint;
using unicode::ExpectNextUTF8;
using unicode::ReadNextUTF8;

namespace json {

#if 0
// The size in bytes the json types will take in a vector.
static size_t VECTOR_TYPE_SIZES[] = {
    // Warning: vector<bool> has to be handed specially because bool vectors pack
    // the bools, so you can't get at individual elements using byte offsets.
    ~0ul,

    sizeof(int8_t),
    sizeof(uint8_t),
    sizeof(int16_t),
    sizeof(uint16_t),
    sizeof(int32_t),
    sizeof(uint32_t),
    sizeof(int64_t),
    sizeof(uint64_t),
    sizeof(size_t),

    sizeof(float),
    sizeof(double),

    sizeof(char*),
    sizeof(string),

    sizeof(string)
};
#endif

// -----------------------------------------------------------------------------
// To JSON.

AppendType APPEND_TYPE[] = {
    &AppendBool,

    &AppendInt<int8_t>,
    &AppendInt<uint8_t>,
    &AppendInt<int16_t>,
    &AppendInt<uint16_t>,
    &AppendInt<int32_t>,
    &AppendInt<uint32_t>,
    &AppendInt<int64_t>,
    &AppendInt<uint64_t>,
    &AppendInt<size_t>,

    &AppendFloat<float>,
    &AppendFloat<double>,

    &AppendChars,
    &AppendStr,

    &AppendObject,
};

static void EscapeAndQuote(string* s) {
    string s2;
    String::Replace(*s, "\"", "\\\"", &s2);
    *s = '"' + s2 + '"';
}

void AppendBool(const void* p, string* r) {
    bool b = *static_cast<const bool*>(p);
    *r += b ? "true" : "false";
}

void AppendChars(const void* p, string* r) {
    string s = *static_cast<const char* const*>(p);
    EscapeAndQuote(&s);
    *r += s;
}

void AppendStr(const void* p, string* r) {
    string s = *static_cast<const string*>(p);
    EscapeAndQuote(&s);
    *r += s;
}

void AppendObject(const void* p, string* r) {
    const string& s = *static_cast<const string*>(p);
    *r += s;
}

void AppendEntry(const Entry& e, string* r) {
    AppendStr(static_cast<const void*>(&e.key), r);
    *r += ':';
    APPEND_TYPE[e.type](e.value, r);
}

void EntriesToJSON(const vector<Entry>& entries, string* r) {
    r->clear();
    *r += '{';
    for (size_t i = 0; i < entries.size(); ++i) {
        AppendEntry(entries[i], r);
        if (i < entries.size() - 1) {
            *r += ',';
        }
    }
    *r += '}';
}

void ToJSON(string* s) {
    vector<Entry> v;
    EntriesToJSON(v, s);
}

// -----------------------------------------------------------------------------
// From JSON.

ParseType PARSE_TYPE[] = {
    ParseBool,

    ParseInt<int8_t>,
    ParseInt<uint8_t>,
    ParseInt<int16_t>,
    ParseInt<uint16_t>,
    ParseInt<int32_t>,
    ParseInt<uint32_t>,
    ParseInt<int64_t>,
    ParseInt<uint64_t>,
    ParseInt<size_t>,

    ParseFloat<float>,
    ParseFloat<double>,

    ParseChars,
    ParseStr,

    ParseObject
};

bool IsFieldEndChar(CodePoint c) {
    return c == ',' || c == ']' || c == '}';
}

void ParseNonquotedNonescapedField(const string& s, size_t* x, string* field) {
    size_t begin = *x;
    CodePoint c;
    while (ReadNextUTF8(s, x, &c)) {
        if (IsFieldEndChar(c)) {
            //--(*x);  // The end chars are all one byte long.
            break;
        }
    }
    *field = s.substr(begin, *x - begin + 1);
}

bool ParseBool(const string& s, size_t* x, void* p) {
    size_t begin = *x;
    while (*x < s.size() && !IsFieldEndChar(static_cast<CodePoint>(s[*x]))) {
        ++(*x);
    }
    const string sub = s.substr(begin, *x - begin);
    if (sub == "true") {
        *static_cast<bool*>(p) = true;
    } else if (sub == "false") {
        *static_cast<bool*>(p) = false;
    } else {
        return false;
    }
    return true;
}

bool ParseQuotedAndEscaped(const string& s, size_t* x, string* field) {
    CodePoint prev_c = static_cast<CodePoint>(~0u);
    CodePoint c;
    if (!ReadNextUTF8(s, x, &c)) {
        return false;
    }
    if (c != '"') {
        return false;
    }
    while (ReadNextUTF8(s, x, &c)) {
        if (c == '"' && prev_c != '\\') {
            break;
        } else {
            AppendUTF8(c, field);
        }
        prev_c = c;
    }
    return true;
}

bool ParseChars(const string& s, size_t* x, void* p) {
    string field;
    if (!ParseQuotedAndEscaped(s, x, &field)) {
        return false;
    }

    size_t z = 0;
    for (; z < field.size(); ++z) {
        if (!field[z]) {
            break;
        }
    }

    char* ret = new char[z + 1];

    for (size_t i = 0; i < z; ++i) {
        ret[i] = field[i];
    }
    ret[z] = '\0';

    char** ptr = static_cast<char**>(p);
    *ptr = ret;

    return true;
}

bool ParseStr(const string& s, size_t* x, void* p) {
    string field;
    if (!ParseQuotedAndEscaped(s, x, &field)) {
        return false;
    }
    *static_cast<string*>(p) = field;
    return true;
}

bool ParseObject(const string& s, size_t* x, void* p) {
    size_t begin = *x;
    CodePoint c;
    if (!ReadNextUTF8(s, x, &c)) {
        return false;
    }
    if (c != '{' && c != '[') {
        return false;
    }
    int depth = 1;
    while (ReadNextUTF8(s, x, &c)) {
        if (c == '{' || c == '[') {
            ++depth;
        } else if (c == '}' || c == ']') {
            --depth;
        }
        if (!depth) {
            break;
        }
    }
    *static_cast<string*>(p) = s.substr(begin, *x - begin);
    return true;
}

bool IsWhitespaceCharacter(CodePoint c) {
    return c == ' ' || c == '\n' || c == '\r';
}

void RemoveWhitespace(const string& in, string* out) {
    size_t x = 0;
    CodePoint c;
    bool in_quotes = false;
    while (ReadNextUTF8(in, &x, &c)) {
        if (in_quotes) {
            if (c == '"') {
                in_quotes = false;
            }
            AppendUTF8(c, out);
        } else {
            if (c == '"') {
                in_quotes = true;
            }
            if (!IsWhitespaceCharacter(c)) {
                AppendUTF8(c, out);
            }
        }
    }
    AppendUTF8(c, out);
}

bool ReadKeyValue(const string& s, Type type, size_t* x, string* key,
                                    void* value) {
    // Read key.
    if (!ParseStr(s, x, static_cast<void*>(key))) {
        return false;
    }

    // Read ':'.
    CodePoint c;
    if (!ExpectNextUTF8(s, x, &c, ':')) {
        return false;
    }

    // Read value.
    if (!PARSE_TYPE[type](s, x, value)) {
        return false;
    }

    return true;
}

bool EntriesFromJSON(const string& orig_s, const map<string, Entry>& m) {
    // Remove whitespace.
    string s;
    RemoveWhitespace(orig_s, &s);

    // Read "{".
    size_t x = 0;
    CodePoint c;
    if (!ExpectNextUTF8(s, &x, &c, '{')) {
        ERROR("[JSON] Expected { is missing.\n");
        return false;
    }

    // For each entry,
    for (size_t i = 0; i < m.size(); ++i) {
        // Read the key.
        string key;
        if (!ParseStr(s, &x, static_cast<void*>(&key))) {
            ERROR("[JSON] Parsing field key failed.\n");
            return false;
        }

        // Read ':'.
        if (!ExpectNextUTF8(s, &x, &c, ':')) {
            ERROR("[JSON] Expected colon is missing.\n");
            return false;
        }

        // Get the type of our value to parse from the provided map.
        auto it = m.find(key);
        if (it == m.end()) {
            ERROR("[JSON] Unknown key: [%s].\n", key.c_str());
            return false;
        }
        const Entry& entry = it->second;

        // Read value.
        if (!PARSE_TYPE[entry.type](s, &x, entry.value)) {
            ERROR("[JSON] Parse type failed.\n");
            return false;
        }

        // Read ",".
        if (i < m.size() - 1) {
            if (!ExpectNextUTF8(s, &x, &c, ',')) {
                ERROR("[JSON] Expected comma is missing.\n");
                return false;
            }
        }
    }

    // Read "}", the last character in the string.
    if (ReadNextUTF8(s, &x, &c)) {
        ERROR("[JSON] Input ended prematurely.\n");
        return false;
    }
    if (c != '}') {
        ERROR("[JSON] Expected } is missing.\n");
        return false;
    }
    return true;
}

bool FromJSON(const string& s) {
    map<string, Entry> m;
    return EntriesFromJSON(s, m);
}

// -----------------------------------------------------------------------------

}  // namespace json
