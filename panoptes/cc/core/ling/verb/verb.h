#ifndef CC_CORE_LING_VERB_VERB_H_
#define CC_CORE_LING_VERB_VERB_H_

#include <string>

#include "cc/base/enum_strings.h"
#include "cc/base/throol.h"

using std::string;

// -----------------------------------------------------------------------------
// Polarity.

class Polarity {
  public:
    bool tf() const { return tf_; }
    throol is_contrary() const { return is_contrary_; }

    void Init(bool p, throol ic);

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

  private:
    // "You said" vs "You didn't say".
    bool tf_;

    // Contrary to the expectation (eg. "you said X" vs "you *did* say X").
    throol is_contrary_;
};

// -----------------------------------------------------------------------------
// Aspect.
//
// Only perfect and progressive are supported, as we are interested in things
// that change how verbs are rendered.

class Aspect {
  public:
    bool is_perf() const { return is_perf_; }
    bool is_prog() const { return is_prog_; }

    void Init(bool is_perf, bool is_prog);

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

  private:
    bool is_perf_;
    bool is_prog_;
};

// -----------------------------------------------------------------------------
// Modality (things that moods and modals indicate).
//
// There are other modality-like meanings performed by regular verbs and
// adverbs, but those are not the concern of this (are about higher-level
// meaning).
//
// Instead of 'conditional' being a modality, the modalities below have normal
// and conditional forms.  This is because of relations like "can + would =
// could".
//
// The two subjunctives:
// - SUBJUNCTIVE_CF (counterfactual) "if he [were] smart"
// - SUBJUNCTIVE_IMP (imperative) "he requests you [come]"

enum ModalFlavor {
    MF_INDICATIVE,
    MF_SUBJUNCTIVE_CF,

    MF_DEDUCTIVE,
    MF_ALMOST_CERTAIN,
    MF_PROBABLE,
    MF_POSSIBLE,

    MF_IMPERATIVE,
    MF_SUBJUNCTIVE_IMP,
    MF_ABILITY,
    MF_PERMISSIVE,
    MF_NORMATIVE,
    MF_NECESSITY,

    MF_NUM_FLAVORS
};

extern EnumStrings<ModalFlavor> ModalFlavorStrings;

class Modality {
  public:
    ModalFlavor flavor() const { return flavor_; }
    bool is_cond() const { return is_cond_; }

    void Init(ModalFlavor f, bool ic);

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

  private:
    ModalFlavor flavor_;
    bool is_cond_;
};

// -----------------------------------------------------------------------------
// Verb.

// Abstract tense that is actually about time only.
enum Tense {
    T_PAST,
    T_PRESENT,
    T_FUTURE,
    T_NUM_TENSES
};

extern EnumStrings<Tense> TenseStrings;

// Deep verb form distinguishes gerund for usage "higher" up in the codebase.
enum VerbForm {
    VF_FINITE,
    VF_BARE_INF,
    VF_TO_INF,
    VF_GERUND,
    VF_SUBJLESS_GERUND,
    VF_NUM_VERB_FORMS
};

extern EnumStrings<VerbForm> VerbFormStrings;

class Verb {
  public:
    const string& lemma() const { return lemma_; }
    const Polarity& polarity() const { return polarity_; }
    Tense tense() const { return tense_; }
    const Aspect& aspect() const { return aspect_; }
    const Modality& modality() const { return modality_; }
    VerbForm verb_form() const { return verb_form_; }
    bool is_pro_verb() const { return is_pro_verb_; }

    void set_lemma(const string& new_lemma);
    void set_is_pro_verb(bool is_pro_verb);

    void Init(const string& l, const Polarity& p, Tense t, const Aspect& a,
                        const Modality& m, VerbForm vf, bool ipv);

    bool IsFinite() const;

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

    bool HasUnsetFields() const;

  private:
    string lemma_;  // Blank if pro-verb.
    Polarity polarity_;
    Tense tense_;
    Aspect aspect_;
    Modality modality_;
    VerbForm verb_form_;
    bool is_pro_verb_;
};

// -----------------------------------------------------------------------------

#endif  // CC_CORE_LING_VERB_VERB_H_
