#include "surface_verb_sayer.h"

#include "cc/base/file.h"
#include "cc/base/table_util.h"

bool SurfaceVerbSayer::Init(
        const Conjugator* c, const string& modal_past_tense_f) {
    conjugator_ = c;

    string s;
    File::FileToString(modal_past_tense_f, &s);
    return conv_.Init(s);
}

void SurfaceVerbSayer::SaySbjFut(
        const SurfaceVerb& v, vector<VerbField>* ff) const {
    Conjugation use_conj = v.use_were_sbj() ? CONJ_P2 : v.conj();
    const string& were_or_was = conjugator_->to_be().past()[use_conj];
    ff->insert(ff->begin(), VerbField("to", 0));
    ff->insert(ff->begin(), VerbField(were_or_was, 0));

    if (v.whether() == WHETHER_NO) {
        long not_x = v.split_inf() ? 2 : 1;
        ff->insert(ff->begin() + not_x, VerbField("not", 0u));
    }
}

void SurfaceVerbSayer::SayNormal(
        const SurfaceVerb& v, const ConjugationSpec& to_verb, bool use_perf,
        vector<VerbField>* ff) const {
    // Add "do".
    if (v.IsFinite()) {
        if (v.whether() == WHETHER_NO || v.whether() == WHETHER_EMPH) {
            bool mood_is_ok = v.mood() == MOOD_IND;
            bool already_has_an_aux =
                (v.modal().size() || use_perf || v.aspect().is_prog());
            bool can_use_do = to_verb.has_do_support() &&
                              v.voice() == SV_ACTIVE;
            if (mood_is_ok && !already_has_an_aux && can_use_do) {
                ff->insert(ff->begin(), VerbField("do", ~0u));
            }
        }
    }

    // Add "to".
    if (v.verb_form() == SVF_TO_INF) {
        ff->insert(ff->begin(), VerbField("to", 0));
    }

    // Add "not".
    if (v.whether() == WHETHER_NO) {
        // Decide where to put the "not".
        long not_x;
        if (v.IsFinite()) {
            if (v.mood() == MOOD_IND || v.mood() == MOOD_SBJ_CF) {
                not_x = 1;
            } else {
                not_x = 0;
            }
        } else {
            if (v.verb_form() == SVF_TO_INF) {
                not_x = v.split_inf() ? 1 : 0;
            } else {
                not_x = 0;
            }
        }

        // Actually insert it.
        ff->insert(ff->begin() + not_x, VerbField("not", 0u));
    }

    // Conjugate the front word depending on mood, tense, etc. if it isn't a
    // modal, "not", "to", etc.  Leftovers are automatically converted to their
    // lemmas (base form - bare infinitive), so in a few cases we just do
    // nothing.
    if (v.IsFinite()) {
        if (v.mood() == MOOD_IND) {
            size_t conj_x = 0;  // If there is a not, it's at index 1.
            if ((*ff)[conj_x].field_index == ~0u) {
                if (v.tense() == ST_NONPAST) {
                    (*ff)[conj_x].field_index = 3 + v.conj();  // Nonpast.
                } else {
                    (*ff)[conj_x] .field_index = 9 + v.conj();  // Past.
                }
            }
        } else if (v.mood() == MOOD_SBJ_CF) {
            if (v.tense() == ST_SBJ_PAST) {
                Conjugation use_conj = v.use_were_sbj() ? CONJ_P2 : v.conj();
                (*ff)[0].field_index = 9 + use_conj;  // Past.
            }
        }
    } else {
        if (v.verb_form() == SVF_GERUND) {
            size_t conj_x = v.whether() == WHETHER_NO ? 1 : 0;
            (*ff)[conj_x].field_index = 1;  // Present participle.
        }
    }
}

VerbSayStatus SurfaceVerbSayer::Say(
        const SurfaceVerb& v, vector<string>* rr) const {
    VerbSayStatus err = conv_.MightBeValid(v);
    if (err != VSS_OK) {
        return err;
    }

    // The past tense of "can" is "could", etc.
    string use_modal;
    bool use_perf;
    if (!conv_.HandleModalPastTense(v.modal(), v.tense(), v.aspect().is_perf(),
                                    &use_modal, &use_perf)) {
        return VSS_INVALID_MODAL_IS_UNKNOWN;
    }

    // Create the conjugation plan for the verb.
    ConjugationSpec to_verb;
    conjugator_->CreateVerbSpec(v.lemma(), &to_verb);

    // List the verb specs to pick the correct forms of.
    vector<VerbField> ff;
    if (use_modal.size()) {
        ff.emplace_back(VerbField(use_modal, 0u));
    }
    if (use_perf) {
        ff.emplace_back(VerbField("have", ~0u));
    }
    if (v.aspect().is_prog()) {
        ff.emplace_back(VerbField("be", ~0u));
    }
    if (v.voice() == SV_PASSIVE) {
        ff.emplace_back(VerbField("be", ~0u));
    }
    ff.emplace_back(VerbField(v.lemma(), ~0u));

/*
    printf("Initial choices:\n");
    for (unsigned i = 0; i < ff.size(); ++i) {
        ff[i].Dump();
    }
    printf("\n");
*/

    // MOOD_SBJ_FUT uses the to_be future, unlike anything else below, so we do
    // it separately here.
    if (v.mood() == MOOD_SBJ_CF && v.tense() == ST_SBJ_FUT) {
        SaySbjFut(v, &ff);
    } else {
        SayNormal(v, to_verb, use_perf, &ff);
    }

    // Get the index of the end of the infinitives (exclusive).
    size_t z = ff.size();

    // If passive voice, use past participle of the last verb.
    if (v.voice() == SV_PASSIVE) {
        --z;
        ff[z].field_index = 2;
    }

    // Conjugate for aspect on the preceding words, if applicable.
    if (v.aspect().is_prog()) {
        --z;
        ff[z].field_index = 1;
    }
    if (use_perf) {
        --z;
        ff[z].field_index = 2;
    }

    // The remaining verb words in the middle are left in lemma form.
    for (unsigned i = 0; i < z; ++i) {
        if (ff[i].field_index == ~0u) {
            ff[i].field_index = 0u;
        }
    }

/*
    printf("Printing resulting choices:\n");
    for (unsigned i = 0; i < ff.size(); ++i) {
        ff[i].Dump();
    }
    printf("\n");
*/

    // Render the words.
    rr->clear();
    rr->reserve(ff.size());
    for (unsigned i = 0; i < ff.size(); ++i) {
        string s;
        conjugator_->Conjugate(ff[i].lemma, ff[i].field_index, &s);
        rr->emplace_back(s);
    }

    // There are two kinds of finite.  Make modifications for the weird kind of
    // finite if necessary where you drop a passivization word when the relative
    // pronoun is a zero (eg. "the cat that [was seen] by you" vs "the cat
    // [seen] by you").  Passives only!
    if (v.verb_form() == SVF_ZERO_RELCLAUSE_FINITE && v.voice() == SV_PASSIVE) {
          const string& s = (*rr)[0];
          if (s == "is" || s == "are" || s == "was" || s == "were") {
                rr->erase(rr->begin());
          }
    }

    return VSS_OK;
}

// -----------------------------------------------------------------------------
