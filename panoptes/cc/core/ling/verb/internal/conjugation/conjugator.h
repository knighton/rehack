#ifndef CC_VERB_INTERNAL_CONJUGATION_CONJUGATOR_H_
#define CC_VERB_INTERNAL_CONJUGATION_CONJUGATOR_H_

#include <map>
#include <string>
#include <vector>

#include "cc/ds/generalizing_suffix_tree.h"
#include "cc/core/ling/verb/internal/conjugation/conjugation_spec.h"
#include "cc/core/ling/verb/internal/conjugation/suffix_transform.h"

using std::map;
using std::string;
using std::vector;

struct LemmaAndIndex {
    string lemma;
    uint8_t index;
    LemmaAndIndex(const string& _lemma, uint8_t _index);
};

typedef uint64_t Hash;

// -----------------------------------------------------------------------------

// A mapping of a verb lemma to its set of conjugations (and back).
class ConjSpecDerivation {
  public:
    // Construct from an example verb.
    void Init(const ConjugationSpec& spec);

    void InitUsingTransformCache(
        const ConjugationSpec& spec,
        map<string, SuffixTransform>* before_after2transform);

    // Get all the conjugations of a verb.
    void Derive(const string& lemma, ConjugationSpec* spec);

    // Given a conjugated word, return what it could be.
    void IdentifyWord(const string& conjugated,
                      vector<LemmaAndIndex>* lemmas_idxs) const;

    Hash HashCode() const;

  private:
    SuffixTransform pres_part_;
    SuffixTransform past_part_;

    // A lot of parts of the verb are derived in the same way, typically two for
    // nonpast and one for past.
    vector<SuffixTransform> nonpast_;
    vector<SuffixTransform> past_;
};

// -----------------------------------------------------------------------------

class Conjugator {
  public:
    const ConjugationSpec& to_be() const { return to_be_; }
    const ConjugationSpec& to_have() const { return to_have_; }
    const ConjugationSpec& to_do() const { return to_do_; }

    bool InitFromConfig(const ConjugationSpecConfig& specs);

    bool InitFromFile(const string& conjugations_f);

    // lemma -> spec.
    void CreateVerbSpec(const string& lemma, ConjugationSpec* spec) const;

    // (lemma, index) -> conjugated.
    void Conjugate(const string& lemma, unsigned field_index,
                   string* conjugated) const;

    // conjugated -> list of (lemma, index).
    void IdentifyWord(const string& conjugated, bool is_picky_about_verbs,
                      vector<LemmaAndIndex>* lemmas_idxs) const;

    void DumpToString(string* s) const;

  private:
    // lemma -> index in derivs_.
    vector<ConjSpecDerivation> derivs_;
    map<string, size_t> lemma2derivx_;

    // Map verb lemmas by suffix to indexes that point to ConjSpecDerivations.
    GeneralizingSuffixTree<size_t> suffix_tree_;

    // Precomputed auxiliary verbs.
    // * to_be contracts in main and auxiliary verb forms the same way.
    // * to_have forms contractions differently as an auxiliary from a main verb
    //   (as a main verb has-'s would conflict with is-'s).
    // * to_do never contracts except for a few weird cases (where did-'d does
    //   not conflict with would-'d).
    ConjugationSpec to_be_;
    ConjugationSpec to_have_;  // Annotated as aux.
    ConjugationSpec to_do_;
};

// -----------------------------------------------------------------------------

#endif  // CC_VERB_INTERNAL_CONJUGATION_CONJUGATOR_H_
