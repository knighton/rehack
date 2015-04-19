#include "conjugation_spec.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

#include "cc/base/logging.h"
#include "cc/base/string.h"

using std::map;
using std::string;
using std::vector;

// -----------------------------------------------------------------------------

void ConjugationSpec::Init(
        const string& lemma, const string& pres_part, const string& past_part,
        const vector<string>& nonpast, const vector<string>& past) {
    lemma_ = lemma;
    pres_part_ = pres_part;
    past_part_ = past_part;
    nonpast_ = nonpast;
    past_ = past;
    has_do_support_ = (lemma != "be");
}

void ConjugationSpec::AnnotateAsAux() {
    for (unsigned i = 0; i < nonpast_.size(); ++i) {
        nonpast_[i] = "<" + nonpast_[i] + "-aux>";
    }
    for (unsigned i = 0; i < past_.size(); ++i) {
        past_[i] = "<" + past_[i] + "-aux>";
    }
}

const string& ConjugationSpec::GetField(unsigned f) const {
    switch (f) {
    case 0:
        return lemma_;
    case 1:
        return pres_part_;
    case 2:
        return past_part_;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return nonpast_[f - 3u];
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
        return past_[f - 9u];
    default:
        assert(false);
        return lemma_;
    }
}

void ConjugationSpec::ToString(string* s) {
    s->clear();
    *s += lemma_ + '\t' + pres_part_ + '\t' + past_part_ + '\t';
    if (nonpast_.size()) {
        *s += nonpast_[0];
    }
    for (size_t j = 1; j < nonpast_.size(); ++j) {
        *s += '|' + nonpast_[j];
    }
    if (past_.size()) {
        *s += '\t' + past_[0];
    }
    for (size_t j = 1; j < past_.size(); ++j) {
        *s += '|' + past_[j];
    }
}

void ConjugationSpec::FromString(const string& s) {
    vector<string> pieces;
    String::Split(s, '\t', &pieces);
    string lemma = pieces[0];
    string pres_part = pieces[1];
    string past_part = pieces[2];
    vector<string> nonpast;
    String::Split(pieces[3], '|', &nonpast);
    vector<string> past;
    String::Split(pieces[4], '|', &past);
    Init(lemma, pres_part, past_part, nonpast, past);
}

// -----------------------------------------------------------------------------

void FlatConjSpec::ToConjSpec(ConjugationSpec* spec) {
    vector<string> nonpast;
    for (unsigned i = 3; i < 9; ++i) {
        nonpast.emplace_back(words_[i]);
    }
    vector<string> past;
    for (unsigned i = 9; i < 15; ++i) {
        past.emplace_back(words_[i]);
    }
    spec->Init(words_[0], words_[1], words_[2], nonpast, past);
}

void FlatConjSpec::FromConjSpec(const ConjugationSpec& spec) {
    words_.resize(1 + 1 + 1 + 6 + 6);
    words_[0] = spec.lemma();
    words_[1] = spec.pres_part();
    words_[2] = spec.past_part();
    for (size_t i = 0; i < 6; ++i) {
        words_[3 + i] = spec.nonpast()[i];
    }
    for (size_t i = 0; i < 6; ++i) {
        words_[9 + i] = spec.past()[i];
    }
    s2xx_.clear();
    for (unsigned i = 0; i < words_.size(); ++i) {
        s2xx_[words_[i]].emplace_back(i);
    }
}

// -----------------------------------------------------------------------------

void ConjugationSpecConfig::ToString(string* s) {
    s->clear();
    if (specs_.size()) {
        string tmp;
        specs_[0].ToString(&tmp);
        *s += tmp;
    }
    for (unsigned i = 0; i < specs_.size(); ++i) {
        *s += '\n';
        string tmp;
        specs_[i].ToString(&tmp);
        *s += tmp;
    }
}

void ConjugationSpecConfig::FromString(const string& s) {
    specs_.clear();
    vector<string> v;
    String::Split(s, '\n', &v);
    for (unsigned i = 0; i < v.size() - 1; ++i) {
        ConjugationSpec spec;
        spec.FromString(v[i]);
        specs_.emplace_back(spec);
    }
    INFO("[ConjugationSpecConfig] Loaded %zu verb conjugation specs.\n",
         specs_.size());
}

// -----------------------------------------------------------------------------
