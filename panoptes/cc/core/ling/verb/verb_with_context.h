#ifndef CC_CORE_LING_VERB_VERB_WITH_CONTEXT_H_
#define CC_CORE_LING_VERB_VERB_WITH_CONTEXT_H_

#include "cc/base/enum_strings.h"
#include "cc/core/ling/misc/inflections.h"
#include "cc/core/ling/verb/verb.h"

// Voice.
enum Voice {
    V_ACTIVE,
    V_PASSIVE,
    V_NUM_VOICES
};

extern EnumStrings<Voice> VoiceStrings;

// Relative containment.
//
// Verbs conjugate slightly differently (certain words being left out) if inside a
// 'zero' relative pronoun relative clause.
//
// Types:
//
// - ZERO
//   - Meaning: is the verb of a relative clause; relpro is a zero.
//   - verb_form restriction: must be finite.
//   - Surface verb_form handling: use SurfaceVerbForm.ZERO_RELCLAUSE_FINITE instead
//     of SurfaceVerbForm.NORMAL_FINITE (eg, "the cat [seen] by you" vs {the cat
//     that [was seen] by you").
//
// - WORD
//   - Meaning: is the verb of a relative clause; relpro is not a zero.
//   - verb_form restriction: must be finite.
//   - Surface verb_form handling: use SurfaceVerbForm.NORMAL_FINITE.
//
// - NO
//   - Meaning: not the verb of a relative clause.
//   - verb_form restriction: any is ok.
//   - Surface verb_form handling; normal (NORMAL_FINITE for finite).
enum RelativeContainment {
    RC_ZERO,
    RC_WORD,
    RC_NO,
    RC_NUM_REL_CONTS
};

extern EnumStrings<RelativeContainment> RelativeContainmentStrings;

// Subjunctive handling ("If I were" vs "If I was").
enum SubjunctiveHandling {
    SH_WERE_SBJ,
    SH_WAS_SBJ,
    SH_NUM_SBJ_HANDLINGS
};

extern EnumStrings<SubjunctiveHandling> SubjunctiveHandlingStrings;

class VerbWithContext {
  public:
    const Verb& verb() const { return verb_; }
    Voice voice() const { return voice_; }
    Conjugation conj() const { return conj_; }
    bool is_split() const { return is_split_; }
    RelativeContainment relative_cont() const { return relative_cont_; }
    throol contract_not() const { return contract_not_; }
    throol split_inf() const { return split_inf_; }
    SubjunctiveHandling sbj_handling() const { return sbj_handling_; }

    void set_lemma(const string& new_lemma);

    void Init(const Verb& v, Voice voice, Conjugation conj, bool is_split,
              RelativeContainment rc, throol contract_not, throol split_inf,
              SubjunctiveHandling sh);

    void InitFromVector(const vector<uint8_t>& values,
                        const vector<string>& lemmas);

    void InitFromVWC(const VerbWithContext& other, Conjugation new_conj);

    bool IsFinite() const;

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

    bool HasUnsetFields() const;

    bool IsPossibleAtSentenceRoot() const;

  private:
    Verb verb_;
    Voice voice_;
    Conjugation conj_;
    bool is_split_;
    RelativeContainment relative_cont_;

    // If the verb words are split, which side does the 'not' go on ("Did you
    // not?" vs "Didn't you?")?
    throol contract_not_;

    // Whether to split infinitives ("to not go" vs "not to go").
    throol split_inf_;

    SubjunctiveHandling sbj_handling_;
};

#endif  // CC_CORE_LING_VERB_VERB_WITH_CONTEXT_H_
