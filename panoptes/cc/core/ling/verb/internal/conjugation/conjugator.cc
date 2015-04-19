#include "conjugator.h"

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "cc/base/file.h"
#include "cc/base/string.h"

using std::hash;
using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

LemmaAndIndex::LemmaAndIndex(const string& _lemma, uint8_t _index) {
    lemma = _lemma;
    index = _index;
}

// -----------------------------------------------------------------------------

void ConjSpecDerivation::Init(const ConjugationSpec& spec) {
    pres_part_.InitFromExample(spec.lemma(), spec.pres_part());
    past_part_.InitFromExample(spec.lemma(), spec.past_part());
    nonpast_.resize(spec.nonpast().size());
    for (unsigned i = 0; i < spec.nonpast().size(); ++i) {
        nonpast_[i].InitFromExample(spec.lemma(), spec.nonpast()[i]);
    }
    past_.resize(spec.past().size());
    for (unsigned i = 0; i < spec.past().size(); ++i) {
        past_[i].InitFromExample(spec.lemma(), spec.past()[i]);
    }
}

namespace {

void SetOne(const string& lemma, const string& conjugated,
            SuffixTransform* transform, map<string, SuffixTransform>* cache) {
    string key = lemma + ":" + conjugated;
    map<string, SuffixTransform>::iterator it = cache->find(key);
    if (it == cache->end()) {
        transform->InitFromExample(lemma, conjugated);
        (*cache)[key] = *transform;
    } else {
        *transform = it->second;
    }
}

}  // namespace

void ConjSpecDerivation::InitUsingTransformCache(
        const ConjugationSpec& spec, map<string, SuffixTransform>* ba2t) {
    string before = spec.lemma() + ":";
    string key = before + spec.pres_part();
    SetOne(spec.lemma(), spec.pres_part(), &pres_part_, ba2t);
    SetOne(spec.lemma(), spec.past_part(), &past_part_, ba2t);
    nonpast_.resize(spec.nonpast().size());
    for (unsigned i = 0; i < nonpast_.size(); ++i) {
        SetOne(spec.lemma(), spec.nonpast()[i], &(nonpast_[i]), ba2t);
    }
    past_.resize(spec.past().size());
    for (unsigned i = 0; i < past_.size(); ++i) {
        SetOne(spec.lemma(), spec.past()[i], &(past_[i]), ba2t);
    }
}

void ConjSpecDerivation::Derive(const string& lemma, ConjugationSpec* spec) {
    string pres_part;
    string past_part;
    vector<string> nonpast;
    vector<string> past;
    pres_part_.Transform(lemma, &pres_part);
    past_part_.Transform(lemma, &past_part);
    for (unsigned i = 0; i < nonpast_.size(); ++i) {
        string s;
        nonpast_[i].Transform(lemma, &s);
        nonpast.emplace_back(s);
    }
    for (unsigned i = 0; i < past_.size(); ++i) {
        string s;
        past_[i].Transform(lemma, &s);
        past.emplace_back(s);
    }
    spec->Init(lemma, pres_part, past_part, nonpast, past);
}

void ConjSpecDerivation::IdentifyWord(
        const string& conjugated, vector<LemmaAndIndex>* lemmas_idxs) const {
    string lemma;
    if (pres_part_.Reverse(conjugated, &lemma)) {
        lemmas_idxs->emplace_back(LemmaAndIndex(lemma, 1u));
    }
    if (past_part_.Reverse(conjugated, &lemma)) {
        lemmas_idxs->emplace_back(LemmaAndIndex(lemma, 2u));
    }
    for (unsigned i = 0; i < nonpast_.size(); ++i) {
        if (nonpast_[i].Reverse(conjugated, &lemma)) {
            uint8_t index = static_cast<uint8_t>(3u + i);
            lemmas_idxs->emplace_back(LemmaAndIndex(lemma, index));
        }
    }
    for (unsigned i = 0; i < past_.size(); ++i) {
        if (past_[i].Reverse(conjugated, &lemma)) {
            uint8_t index = static_cast<uint8_t>(3u + nonpast_.size() + i);
            lemmas_idxs->emplace_back(LemmaAndIndex(lemma, index));
        }
    }
}

Hash ConjSpecDerivation::HashCode() const {
    string s;
    s += pres_part_.ToString();
    s += past_part_.ToString();
    for (size_t i = 0; i < nonpast_.size(); ++i) {
        s += nonpast_[i].ToString();
    }
    for (size_t i = 0; i < past_.size(); ++i) {
        s += past_[i].ToString();
    }
    hash<string> f;
    return f(s);
}

// -----------------------------------------------------------------------------

static void CollectVerbDerivations(
        const vector<ConjugationSpec>& specs,
        vector<ConjSpecDerivation>* ordered_derivs,
        map<string, size_t>* lemma2ordered_derivx) {
    // The SuffixTransform cache.  Many transformations are the same.
    map<string, SuffixTransform> before_after2transform;

    // List of resulting unique ConjSpecDerivations for all verbs.
    vector<ConjSpecDerivation> derivs;

    // List of hashes corresponding to 'derivs' vector.
    vector<Hash> deriv_hashes;        // deriv index -> hash.
    map<Hash, size_t> derivh2derivx;  // hash -> deriv index.

    // Set of all lemmas seen, for dupe checking.
    set<string> lemmas_set;

    // ConjSpecDerivation hash code -> list of lemmas that derive that way.
    map<Hash, vector<string> > derivh2lemmas;

    // List the unique verb derivations, and the verbs handled by each
    // derivation.
    for (unsigned i = 0; i < specs.size(); ++i) {
        // Dupe verb check.
        const string lemma = specs[i].lemma();
        if (lemmas_set.find(lemma) != lemmas_set.end()) {
            continue;
        }
        lemmas_set.insert(lemma);

        // Create derivation from the spec's fields.
        ConjSpecDerivation deriv;
        deriv.InitUsingTransformCache(specs[i], &before_after2transform);

        // Hash it, check if seen.
        Hash hash = deriv.HashCode();
        map<Hash, vector<string> >::iterator it = derivh2lemmas.find(hash);
        if (it == derivh2lemmas.end()) {
            derivs.emplace_back(deriv);
            deriv_hashes.emplace_back(hash);
            derivh2derivx[hash] = derivs.size() - 1;
            derivh2lemmas[hash].emplace_back(lemma);
        } else {
            it->second.emplace_back(lemma);
        }
    }

    INFO("[Conjugator] There are %zu unique verb spec derivations.\n",
         derivh2lemmas.size());

    // Reorder by usage.  We use min() to select deriv to use in suffix tree.
    vector<pair<size_t, size_t> > counts_derivxs;
    for (map<Hash, vector<string> >::const_iterator it = derivh2lemmas.begin();
         it != derivh2lemmas.end(); ++it) {
        size_t count = it->second.size();
        size_t derivx = derivh2derivx[it->first];
        counts_derivxs.emplace_back(make_pair(count, derivx));
    }
    sort(counts_derivxs.begin(), counts_derivxs.end());

    // Reorder derivs by number of lemmas.
    ordered_derivs->clear();
    map<Hash, size_t> derivh2ordered_derivx;
    for (unsigned i = 0; i < counts_derivxs.size(); ++i) {
        size_t derivx = counts_derivxs[i].second;
        ordered_derivs->emplace_back(derivs[derivx]);
        Hash hash = deriv_hashes[derivx];
        derivh2ordered_derivx[hash] = i;
    }

    // Build size-ordered verb -> derivation mapping.
    lemma2ordered_derivx->clear();
    for (map<Hash, vector<string> >::const_iterator it = derivh2lemmas.begin();
         it != derivh2lemmas.end(); ++it) {
        const vector<string>& lemmas = it->second;
        size_t x = derivh2ordered_derivx[it->first];
        for (unsigned i = 0; i < lemmas.size(); ++i) {
            (*lemma2ordered_derivx)[lemmas[i]] = x;
        }
    }
}

bool Conjugator::InitFromConfig(const ConjugationSpecConfig& config) {
    CollectVerbDerivations(config.specs(), &derivs_, &lemma2derivx_);
    suffix_tree_.InitFromDict(lemma2derivx_);

    // Precompute auxiliary verbs.
    // Relies on suffix tree existing.
    CreateVerbSpec("be", &to_be_);
    CreateVerbSpec("have", &to_have_);
    to_have_.AnnotateAsAux();
    CreateVerbSpec("do", &to_do_);

    return true;
}

bool Conjugator::InitFromFile(const string& conjugations_f) {
    string s;
    if (!File::FileToString(conjugations_f, &s)) {
        return false;
    }

    INFO("[Conjugator] Loading conjugation specs from [%s].\n",
              conjugations_f.c_str());

    ConjugationSpecConfig config;
    config.FromString(s);

    return InitFromConfig(config);
}

void Conjugator::CreateVerbSpec(
        const string& lemma, ConjugationSpec* spec) const {
    // Handle the magic ints verb.
    if (lemma == "<ints>") {
        vector<string> nonpast = {"3", "4", "5", "6", "7", "8"};
        vector<string> past = {"9", "10", "11", "12", "13", "14"};
        spec->Init("0", "1", "2", nonpast, past);
        return;
    }

    // Normal verb spec derivation.
    size_t deriv_index;
    suffix_tree_.Get(lemma, &deriv_index);
    ConjSpecDerivation deriv = derivs_[deriv_index];
    deriv.Derive(lemma, spec);
}

void Conjugator::Conjugate(
        const string& lemma, unsigned field_index, string* conjugated) const {
    // This shortcut allows the don't conjugate trick.
    if (field_index == 0) {
        *conjugated = lemma;
    }

    ConjugationSpec spec;
    CreateVerbSpec(lemma, &spec);
    *conjugated = spec.GetField(field_index);
}

// "bakes" -> [("bake", 5)].
void Conjugator::IdentifyWord(
        const string& conjugated, bool is_picky_about_verbs,
        vector<LemmaAndIndex>* lemmas_idxs) const {
    lemmas_idxs->clear();

    // Aux is special.
    if (String::EndsWith(conjugated, "-aux>")) {
        return;
    }

    // For each derivation, reverse it to the proposed original lemma.
    // If the suffix tree maps that lemma to the derivation we used, it is a
    // hit.
    for (unsigned i = 0; i < derivs_.size(); ++i) {
        vector<LemmaAndIndex> proposed_lemmas_idxs;
        derivs_[i].IdentifyWord(conjugated, &proposed_lemmas_idxs);
        for (unsigned j = 0; j < proposed_lemmas_idxs.size(); ++j) {
            string lemma = proposed_lemmas_idxs[j].lemma;
            size_t deriv_idx = ~0ul;
            suffix_tree_.Get(lemma, &deriv_idx);
            if (deriv_idx == i) {
                uint8_t shorter = static_cast<uint8_t>(deriv_idx);
                lemmas_idxs->emplace_back(LemmaAndIndex(lemma, shorter));
            }
        }
    }

    // Being picky about verbs: if we have some results that contain known
    // verbs, forget about the results with unknown verbs.
    if (is_picky_about_verbs) {
        bool has_known = false;
        for (unsigned i = 0; i < lemmas_idxs->size(); ++i) {
            if (lemma2derivx_.find((*lemmas_idxs)[i].lemma) !=
                lemma2derivx_.end()) {
                has_known = true;
                break;
            }
        }
        if (has_known) {
            vector<LemmaAndIndex> nu;
            for (unsigned i = 0; i < lemmas_idxs->size(); ++i) {
                const string& lemma = (*lemmas_idxs)[i].lemma;
                if (lemma2derivx_.find(lemma) != lemma2derivx_.end()) {
                    nu.emplace_back((*lemmas_idxs)[i]);
                }
            }
            *lemmas_idxs = nu;
        }
    }

    // Try the conjugated word as a lemma itself.
    if (!is_picky_about_verbs ||
            lemma2derivx_.find(conjugated) != lemma2derivx_.end()) {
        lemmas_idxs->emplace_back(LemmaAndIndex(conjugated, 0));
    }
}

void Conjugator::DumpToString(string* s) const {
    suffix_tree_.DumpToString(s);
}

// -----------------------------------------------------------------------------
