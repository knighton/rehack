// Based on https://en.wikipedia.org/wiki/UTF-8.

#include "unicode.h"

#include <cassert>

#include "cc/base/logging.h"

using unicode::CodePoint;

void unicode::AppendUTF8(CodePoint n, string* s) {
    if (n < 0x80) {
        *s += static_cast<char>(n);
        return;
    }

    if (n < 0x800) {
        *s += static_cast<char>((n >> 6) + 0xC0);
        *s += static_cast<char>((n & 0x3F) + 0x80);
        return;
    }

    if (n < 0x10000) {
        *s += static_cast<char>((n >> 12) + 0xE0);
        *s += static_cast<char>(((n >> 6) & 0x3F) + 0x80);
        *s += static_cast<char>((n & 0x3F) + 0x80);
        return;
    }

    if (n < 0x110000) {
        *s += static_cast<char>((n >> 18) + 0xF0);
        *s += static_cast<char>(((n >> 12) & 0x3F) + 0x80);
        *s += static_cast<char>(((n >> 6) & 0x3F) + 0x80);
        *s += static_cast<char>((n & 0x3F) + 0x80);
        return;
    }

    FATAL("Invalid code point: %u.\n", n);
    assert(false);
}

bool unicode::ReadNextUTF8(const string& s, size_t* x, CodePoint* n) {
    if (!(*x < s.size())) {
        return false;
    }

    uint32_t c0;
    uint32_t c1;
    uint32_t c2;
    uint32_t c3;

    c0 = static_cast<uint32_t>(s[*x]);
    ++(*x);

    if (c0 < 0x80) {
        *n = c0;
        return true;
    } else if (c0 < 0xC2) {
        // Continuation or overlong 2-byte sequence.
        goto ERROR1;
    } else if (c0 < 0xE0) {
        // 2-byte sequence.
        c1 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c1 & 0xC0) != 0x80) {
            goto ERROR2;
        }
        *n = (c0 << 6) + c1 - 0x3080;
        return true;
    } else if (c0 < 0xF0) {
        // 3-byte sequence.
        c1 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c1 & 0xC0) != 0x80) {
            goto ERROR2;
        }
        if (c0 == 0xE0 && c1 < 0xA0) {
            goto ERROR2;  // overlong.
        }
        c2 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c2 & 0xC0) != 0x80) {
            goto ERROR3;
        }
        *n = (c0 << 12) + (c1 << 6) + c2 - 0xE2080;
        return true;
    } else if (c0 < 0xF5) {
        // 4-byte sequence.
        c1 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c1 & 0xC0) != 0x80) {
            goto ERROR2;
        }
        if (c0 == 0xF0 && c1 < 0x90) {
            goto ERROR2;  // Overlong.
        }
        if (c0 == 0xF4 && c1 >= 0x90) {
            goto ERROR2;  // > U+10FFFF.
        }
        c2 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c2 & 0xC0) != 0x80) {
            goto ERROR3;
        }
        c3 = static_cast<uint32_t>(s[*x]);
        ++(*x);
        if ((c3 & 0xC0) != 0x80) {
            goto ERROR4;
        }
        *n = (c0 << 18) + (c1 << 12) + (c2 << 6) + c3 - 0x3C82080;
        return true;
    } else {
        // U+10FFFF.
        goto ERROR1;
    }

    ERROR4:
        --(*x);
    ERROR3:
        --(*x);
    ERROR2:
        --(*x);
    ERROR1:
        *n = c0 + 0xDC00;
    return false;
}

bool unicode::ExpectNextUTF8(
        const string& s, size_t* x, CodePoint* n, CodePoint expected) {
    if (!unicode::ReadNextUTF8(s, x, n)) {
        return false;
    }

    if (*n != expected) {
        return false;
    }

    return true;
}
