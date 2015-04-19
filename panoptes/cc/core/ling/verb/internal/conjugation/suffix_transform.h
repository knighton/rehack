#ifndef CC_VERB_INTERNAL_CONJUGATION_SUFFIX_TRANSFORMS_H_
#define CC_VERB_INTERNAL_CONJUGATION_SUFFIX_TRANSFORMS_H_

#include <string>

using std::string;

class SuffixTransform {
  public:
    void InitFromValues(const string& truncate, size_t repeat,
                        const string& append);
    void InitFromExample(const string& from, const string& to);

    // Perform the operation on the string.  Returns false if impossible.
    bool Transform(const string& from, string* to) const;

    // Reverse the operation on the string.  Returns false if impossible.
    bool Reverse(const string& to, string* from) const;

    string ToString() const;

    static size_t FinalRepeatLength(const string& s);

  private:
    string truncate_;
    size_t repeat_;
    string append_;
};

#endif  // CC_VERB_INTERNAL_CONJUGATION_SUFFIX_TRANSFORMS_H_
