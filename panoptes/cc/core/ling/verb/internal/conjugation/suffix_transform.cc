#include "suffix_transform.h"

#include <functional>
#include <string>

#include "cc/base/string.h"

using std::string;

void SuffixTransform::InitFromValues(
        const string& truncate, size_t repeat, const string& append) {
    truncate_ = truncate;
    repeat_ = repeat;
    append_ = append;
}

size_t SuffixTransform::FinalRepeatLength(const string& s) {
    if (!s.size()) {
        return 0;
    }
    char c = s[s.size() - 1];
    for (size_t i = s.size() - 2; i != ~0ul; --i) {
        if (s[i] != c) {
            return s.size() - i - 1;
        }
    }
    return s.size();
}

void SuffixTransform::InitFromExample(const string& from, const string& to) {
    for (size_t i = from.size() - 1; i >= 1; --i) {
        string base = from.substr(0, i);
        for (size_t j = FinalRepeatLength(base); j != ~0ul; --j) {
            size_t repeat = j;
            char repeated = base[i - 1];
            string base_repeat = base + string(repeat, repeated);
            if (to.rfind(base_repeat, 0) == 0) {
                string truncate = from.substr(i);
                string append = to.substr(i + j);
                return InitFromValues(truncate, repeat, append);
            }
        }
    }
    InitFromValues(from, 0, to);
}

bool SuffixTransform::Transform(const string& from, string* to) const {
    if (from.size() < truncate_.size()) {
        return false;
    }
    string r = from.substr(0, from.size() - truncate_.size());
    if (repeat_) {
        if (!r.size()) {
            return false;
        }
        r += string(repeat_, r[r.size() - 1]);
    }
    r += append_;
    *to = r;
    return true;
}

bool SuffixTransform::Reverse(const string& after, string* before) const {
    string r;
    if (!append_.size()) {
        r = after;
    } else {
        if (!String::EndsWith(after, append_)) {
            return false;
        }
        r = after.substr(0, append_.size());
    }
    if (repeat_) {
        string suffix = string(repeat_, r[r.size() - 1]);
        if (!r.size() || !String::EndsWith(r, suffix)) {
            return false;
        }
        r.resize(r.size() - repeat_);
    }
    r += truncate_;
    *before = r;
    return true;
}

string SuffixTransform::ToString() const {
    string s = String::StringPrintf(
        "%s-%d-%s", truncate_.c_str(), repeat_, append_.c_str());
    return s;
}
