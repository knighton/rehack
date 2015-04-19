#ifndef CC_PANOPTES_ENGLISH_vERBS_VERB_SAY_RESULT_H_
#define CC_PANOPTES_ENGLISH_vERBS_VERB_SAY_RESULT_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

struct VerbSayResult {
    vector<string> pre_words;
    vector<string> main_words;

    void ToKey(string* s) const;
    bool FromKey(const string& s);

    void Dump() const;
};

#endif  // CC_PANOPTES_ENGLISH_vERBS_VERB_SAY_RESULT_H_
