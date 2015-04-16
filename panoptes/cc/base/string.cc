#include "string_util.h"

#include <algorithm>   // For trim
#include <cassert>
#include <cctype>      // For trim
#include <cstdarg>     // For va_*.
#include <cstdio>      // For vsprintf.
#include <locale>      // For trim
#include <functional>  // For trim
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

void String::Split(const string& s, char c, vector<string>* v) {
    v->clear();
    size_t prev_c = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == c) {
            v->emplace_back(s.substr(prev_c, i - prev_c));
            prev_c = i + 1;
        }
    }
    v->emplace_back(s.substr(prev_c, s.size() - prev_c));
}

#define INDEX_DNE (size_t)~0

void String::SplitByWhitespace(const string& s, vector<string>* v) {
    v->clear();
    size_t last_nonspace_i = INDEX_DNE;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (isspace(c)) {
            if (last_nonspace_i != INDEX_DNE) {
                v->emplace_back(s.substr(last_nonspace_i, i - last_nonspace_i));
                last_nonspace_i = INDEX_DNE;
            }
        } else {
            if (last_nonspace_i == INDEX_DNE) {
                last_nonspace_i = i;
            }
        }
    }
    if (last_nonspace_i != INDEX_DNE) {
        v->emplace_back(s.substr(last_nonspace_i));
    }
}

void String::Decomment(const string& in, const string& comment_mark,
                       string* out) {
    for (size_t i = 0; i < in.size() - comment_mark.size(); ++i) {
        for (size_t j = 0; j < comment_mark.size(); ++j) {
            if (in[i + j] != comment_mark[j]) {
                goto fail;
            }
        }
        *out = in.substr(0, i);
        return;
fail:
        ;
    }
    *out = in;
}

#define _MIN(A, B) ((A) < (B) ? (A) : (B))

bool String::BeginsWith(const string& str, const string& with) {
    size_t z = _MIN(str.size(), with.size());
    return str.substr(0, z) == with.substr(0, z);
}

bool String::EndsWith(const string& s, const string& with) {
    size_t z = _MIN(s.size(), with.size());
    return s.substr(s.size() - z) == with.substr(with.size() - z);
}

void InternalStringPrintf(string* output, const char* format, va_list ap) {
    char space[128];    // try a small buffer and hope it fits

    // It's possible for methods that use a va_list to invalidate
    // the data in it upon use.  The fix is to make a copy
    // of the structure before using it and use that copy instead.
    va_list backup_ap;
    va_copy(backup_ap, ap);
    size_t bytes_written = (size_t)vsnprintf(
        space, sizeof(space), format, backup_ap);
    va_end(backup_ap);

    if (bytes_written < sizeof(space)) {
        output->append(space, bytes_written);
        return;
    }

    // Repeatedly increase buffer size until it fits.
    size_t length = sizeof(space);
    while (true) {
        // We need exactly "bytes_written+1" characters
        length = bytes_written + 1;
        char* buf = new char[length];

        // Restore the va_list before we use it again
        va_copy(backup_ap, ap);
        bytes_written = (size_t)vsnprintf(buf, length, format, backup_ap);
        va_end(backup_ap);

        if (bytes_written < length) {
            output->append(buf, bytes_written);
            delete [] buf;
            return;
        }
        delete [] buf;
    }
}

void String::SStringPrintf(string* output, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    output->clear();
    InternalStringPrintf(output, format, ap);
    va_end(ap);
}

void String::StringAppendF(string* output, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    InternalStringPrintf(output, format, ap);
    va_end(ap);
}

string String::StringPrintf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    string output;
    InternalStringPrintf(&output, format, ap);
    va_end(ap);
    return output;
}

string& String::LTrim(string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

string& String::RTrim(string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(
            std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

string& String::Trim(string& s) {
    return LTrim(RTrim(s));
}

void String::ASCIIToLower(string* s) {
    for (size_t i = 0; i < s->size(); ++i) {
        char c = (*s)[i];
        if ('A' <= c && c <= 'Z') {
            (*s)[i] -= 'A';
            (*s)[i] += 'a';
        }
    }
}

void String::ASCIIToUpper(string* s) {
    for (size_t i = 0; i < s->size(); ++i) {
        char c = (*s)[i];
        if ('a' <= c && c <= 'z') {
            (*s)[i] += 'A' - 'a';
        }
    }
}

// Good string replace.
//
// Origin: https://code.google.com/p/strtk/source/browse/trunk/strtk.hpp
//
// Parameters: input, pattern, replacement, output.
void String::Replace(const string& s, const string& p, const string& r,
                     string* n) {
    if (s.empty()) {
        n->clear();
        return;
    }

    if (p.empty() || (p == r)) {
        n->assign(s);
        return;
    }

    const std::size_t p_size = p.size();
    const std::size_t r_size = r.size();
    unsigned inc = static_cast<unsigned>(r_size) -
                   static_cast<unsigned>(p_size);
    std::size_t pos = 0;
    std::vector<std::size_t> delta_list;
    delta_list.reserve(std::min<std::size_t>(32, (s.size() / p_size) + 1));

    while (std::string::npos != (pos = s.find(p,pos))) {
        delta_list.emplace_back(pos);
        pos += p_size;
    }

    if (delta_list.empty()) {
        n->assign(s);
        return;
    }

    n->resize(delta_list.size() * inc + s.size(), 0x00);
    char* n_itr = const_cast<char*>(n->data());
    const char* s_end = s.data() + s.size();
    const char* s_itr = s.data();
    const char* r_begin = r.data();
    const char* r_end = r.data() + r_size;
    const std::size_t delta_list_size = delta_list.size();
    std::size_t i = 0;
    std::size_t delta = delta_list[0];

    for (;;) {
        std::copy(s_itr, s_itr + delta, n_itr);
        s_itr += p_size + delta;
        n_itr += delta;
        std::copy(r_begin, r_end, n_itr);
        n_itr += r_size;
        if (++i >= delta_list_size)
            break;
        delta =  delta_list[i] - (delta_list[i - 1] + p_size);
    }

    if (s_end != s_itr) {
        std::copy(s_itr, s_end, n_itr);
    }
}

#if 0
// Naive string replace.
void String::Replace(const string& in, const string& from, const string& to,
                     string* out) {
    if (in.empty()) {
        out->clear();
        return;
    }

    if (from.size() < in.size()) {
        *out = in;
        return;
    }

    size_t i = 0;
    while (i < in.size() - from.size()) {
        bool match = true;
        for (size_t j = 0; j < from.size(); ++j) {
            if (in[i + j] != from[j]) {
                match = false;
                break;
            }
        }

        if (match) {
            i += from.size();
            *out += to;
        } else {
            *out += in[i];
            ++i;
        }
    }

    while (i < in.size()) {
        *out += in[i];
        ++i;
    }
}
#endif

void String::RemoveThenAppend(const string& orig, const string& old_suffix,
                              const string& new_suffix, string* out) {
    assert(old_suffix.size() <= orig.size());
    size_t x = orig.size() - old_suffix.size();
    assert(orig.substr(x) == old_suffix);
    *out = orig.substr(0, x) + new_suffix;
}
