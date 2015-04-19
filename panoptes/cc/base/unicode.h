#ifndef CC_BASE_UNICODE_H_
#define CC_BASE_UNICODE_H_

#include <string>

#include "cc/base/unicode.h"

using std::string;

namespace unicode {

typedef uint32_t CodePoint;

// Append the code point's UTF-8 representation to the string.
void AppendUTF8(CodePoint n, string* s);

// Read the next code point from a UTF-8 encoded string.
//
// Returns false when (a) out of string or (b) invalid data.
bool ReadNextUTF8(const string& s, size_t* x, CodePoint* n);

// Read the next UTF-8 code point.  Returns false if it is not the expected
// character or the read returned false.
bool ExpectNextUTF8(
    const string& s, size_t* x, CodePoint* n, CodePoint expected);

}  // namespace unicode

#endif  // CC_BASE_UNICODE_H_
