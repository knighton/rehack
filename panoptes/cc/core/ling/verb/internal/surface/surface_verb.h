#ifndef CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_H_
#define CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_H_

// Emphermeral object used to say verbs.
//
// Real verbs use modality; here we use modals and moods.  Real verbs use
// abstract tense that refers purely to time; our tense refers to English
// mood-specific tenses.  We have different, surface-specific verb form options,
// etc.

#include <string>

#include "cc/base/throol.h"
#include "cc/core/ling/misc/inflections.h"

using std::string;

// Like polarity, but also "does" as in "she *does* know".
enum Whether {
    WHETHER_NO,
    WHETHER_YES,
    WHETHER_EMPH  // Emphatic yes (yes, and the expectation was no).
};

// Tense.  Gotcha warning: loosely related to time.
enum SurfaceTense {
    // Normal tenses.
    ST_PAST,     // Past.
    ST_NONPAST,  // Present and future (+ will).

    // Subjunctive tenses.
    ST_SBJ_PAST,  // "If I [were] there right now/last year."
    ST_SBJ_PRES,  // "She requests you [come]."
    ST_SBJ_FUT    // "If I [were to become] a sailor."
};

class SurfaceAspect {
  public:
    bool is_perf() const { return is_perf_; }
    bool is_prog() const { return is_prog_; }

    void Init(bool is_perf, bool is_prog);

  private:
    bool is_perf_;
    bool is_prog_;
};

enum Mood {
    MOOD_IND,      // The normal mood: "They [went]."
    MOOD_IMP,      // Imperative: "[Go]."
    MOOD_SBJ_IMP,  // 'Subjunctive-imperative: "She requests you [come]."
    MOOD_SBJ_CF    // Subjunctive-counterfactual: "Wish you [were] here."
};

enum SurfaceVerbForm {
    SVF_NORMAL_FINITE,          // "The cat that [was seen] by me".
    SVF_ZERO_RELCLAUSE_FINITE,  // "The cat [seen] by me".
    SVF_BARE_INF,
    SVF_TO_INF,
    SVF_GERUND
};

enum SurfaceVoice {
    SV_ACTIVE,
    SV_PASSIVE
};

class SurfaceVerb {
  public:
    const string& lemma() const { return lemma_; }
    Whether whether() const { return whether_; }
    SurfaceTense tense() const { return tense_; }
    const SurfaceAspect& aspect() const { return aspect_; }
    const string& modal() const { return modal_; }
    Mood mood() const { return mood_; }
    SurfaceVerbForm verb_form() const { return verb_form_; }
    SurfaceVoice voice() const { return voice_; }
    Conjugation conj() const { return conj_; }
    bool split_inf() const { return split_inf_; }
    bool use_were_sbj() const { return use_were_sbj_; }

    void Init(const string& l, Whether w, SurfaceTense t, const SurfaceAspect& a,
                        const string& modal, Mood mood, SurfaceVerbForm vf, SurfaceVoice v,
                        Conjugation c, bool si, bool uws);

    bool IsFinite() const;

  private:
    string lemma_;
    Whether whether_;
    SurfaceTense tense_;
    SurfaceAspect aspect_;
    string modal_;
    Mood mood_;
    SurfaceVerbForm verb_form_;
    SurfaceVoice voice_;
    Conjugation conj_;
    bool split_inf_;
    bool use_were_sbj_;
};

#endif  // CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_H_
