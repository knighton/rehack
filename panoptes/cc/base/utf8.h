#ifndef CC_BASE_UTF8_H_
#define CC_BASE_UTF8_H_

#include <string>

#include "cc/base/unicode.h"

using std::string;

class UTF8 {
  public:
    // Append the code point's UTF-8 representation to the string.
    static void Append(Unicode::CodePoint n, string* s);

    // Read the next code point from a UTF-8 encoded string.
    //
    // Returns false when (a) out of string or (b) invalid data.
    static bool ReadNext(const string& s, size_t* x, Unicode::CodePoint* n);
};

#endif  // CC_BASE_UTF8_H_
