#ifndef CC_BASE_STRING_H_
#define CC_BASE_STRING_H_

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

class String {
  public:
    // Split a string |s| into pieces separated by |c| into |v|.
    //
    // Examples:
    //   ("Hello, World\n", ',') -> ["Hello", " World\n"]
    //   ("", 'c') -> [""]
    //   ("c", 'c') -> ["", ""]
    //   ("cc", 'c') -> ["", "", ""]
    static void Split(const string& s, char c, vector<string>* v);

    // Split by whitespace.  Ignore whitespace on either end.
    static void SplitByWhitespace(const string& s, vector<string>* v);

    // atoi/atol equivalent for all integral types.
    // Returns bool success.
    template <typename int_t>
    static bool ToInt(const string& from, int_t* to);
    template <typename int_t>
    static bool ToInt(const string& from, size_t* x, int_t* to);

    // atof/atod equivalent for all floating point types.
    // Returns bool success.
    template <typename float_t>
    static bool ToFloat(const string& from, size_t* x, float_t* to);

    // Put text without the comments at end of line if they exist into out.
    static void Decomment(const string& in, const string& comment_mark,
                          string* out);

    // Whether |s| begins with |with|.
    static bool BeginsWith(const string& s, const string& with);

    // Whether |s| ends with |with|.
    static bool EndsWith(const string& s, const string& with);

    // Equivalents of printf that work on strings, from gflags source code.
    // Clears output before writing to it.
    static void SStringPrintf(std::string* output, const char* format, ...);
    static void StringAppendF(std::string* output, const char* format, ...);
    static std::string StringPrintf(const char* format, ...);

    static string& LTrim(string& s);
    static string& RTrim(string& s);
    static string& Trim(string& s);

    static void ASCIIToLower(string* s);
    static void ASCIIToUpper(string* s);

    static void Replace(const string& input, const string& pattern,
                        const string& replacement, string* output);

    static void RemoveThenAppend(const string& orig, const string& old_suffix,
                                 const string& new_suffix, string* out);
};

#include "string_impl.h"

#endif  // CC_BASE_STRING_H_
