#include "surface_verb.h"

// -----------------------------------------------------------------------------

void SurfaceAspect::Init(bool is_perf, bool is_prog) {
    is_perf_ = is_perf;
    is_prog_ = is_prog;
}

// -----------------------------------------------------------------------------

void SurfaceVerb::Init(
        const string& l, Whether w, SurfaceTense t, const SurfaceAspect& a,
        const string& modal, Mood mood, SurfaceVerbForm vf, SurfaceVoice v,
        Conjugation c, bool si, bool uws) {
    lemma_ = l;
    whether_ = w;
    tense_ = t;
    aspect_.Init(a.is_perf(), a.is_prog());
    modal_ = modal;
    mood_ = mood;
    verb_form_ = vf;
    voice_ = v;
    conj_ = c;
    split_inf_ = si;
    use_were_sbj_ = uws;
}

bool SurfaceVerb::IsFinite() const {
    return verb_form_ == SVF_NORMAL_FINITE ||
                  verb_form_ == SVF_ZERO_RELCLAUSE_FINITE;
}

// -----------------------------------------------------------------------------
