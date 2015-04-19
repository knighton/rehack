#include "verb_sayer.h"

#include "cc/base/string.h"

#include <cassert>

// -----------------------------------------------------------------------------

void VerbSayResult::ToKey(string* s) const {
    for (unsigned i = 0; i < pre_words.size(); ++i) {
        *s += pre_words[i];
        if (i < pre_words.size() - 1) {
            *s += ':';
        }
    }
    *s += '|';
    for (unsigned i = 0; i < main_words.size(); ++i) {
        *s += main_words[i];
        if (i < main_words.size() - 1) {
            *s += ':';
        }
    }
}

bool VerbSayResult::FromKey(const string& s) {
    vector<string> pieces;
    String::Split(s, '|', &pieces);
    if (pieces.size() != 2) {
        return false;
    }

    String::Split(pieces[0], ':', &pre_words);
    String::Split(pieces[1], ':', &main_words);
    return true;
}

// -----------------------------------------------------------------------------

bool VerbSayer::Init(const Conjugator* conj, const string& modalities_f,
                     const string& modal_past_tense_f) {
    if (!conv_.Init(modalities_f)) {
        return false;
    };

    if (!surface_.Init(conj, modal_past_tense_f)) {
        return false;
    }

    return true;
}

static void WrapProVerb(string* s) {
    *s = "<" + *s + "-pv>";
}

VerbSayStatus VerbSayer::SliceVerbWords(
        const vector<string>& ss, bool is_split, bool is_pro_verb,
        bool contract_not, VerbSayResult* r) const {
    if (ss.empty()) {
        return VSS_ERR_NO_VERB_WORDS;
    }

/*
    printf("SLICE:");
    for (unsigned i = 0; i < ss.size(); ++i) {
        printf(" (%s)", ss[i].c_str());
    }
    printf(" -- is_split=%d, is_pro_verb=%d contact_not=%d\n", is_split,
           is_pro_verb, contract_not);
*/

    // Some non-finite cases.
    // Eg, "she requests you [not come]."
    bool not_at_0 = 1 <= ss.size() && ss[0] == "not";

    // Normal "not" cases.
    // Eg, "I [do not like] you."
    bool not_at_1 = 2 <= ss.size() && ss[1] == "not";

    // Future subjunctive "not" cases.
    // Eg, "If you [were to not go], ..."
    bool not_at_2 = 3 <= ss.size() && ss[2] == "not";

    // There can be at most one not.
    unsigned n = static_cast<unsigned>(not_at_0) +
                 static_cast<unsigned>(not_at_1) +
                 static_cast<unsigned>(not_at_2);
    if (1 < n) {
        return VSS_ERR_MULTIPLE_NOTS;
    }

    // Determine pre-words (eg, "do" of "[do] you [not like]").
    unsigned split = 0;
    if (is_split) {
        if (not_at_1) {
            if (contract_not) {
                split = 2;
            } else{
                split = 1;
            }
        } else {
            split = 1;
        }
    }

    // Determine rest-words (eg, "not like" of "[do] you [not like]").
    size_t z = ss.size();
    if (is_pro_verb) {
        if (not_at_0) {
            z = 1;
        } else if (not_at_1) {
            z = 2;
        } else if (not_at_2) {
            z = 3;
        } else {
            z = 1;
        }
    }

    // Get the ranges.
    r->pre_words.clear();
    for (unsigned i = 0; i < split; ++i) {
        r->pre_words.emplace_back(ss[i]);
    }
    r->main_words.clear();
    for (size_t i = split; i < z; ++i) {
        r->main_words.emplace_back(ss[i]);
    }

    // If it's a pro-verb, make the words as pro-verb words.
    if (is_pro_verb) {
        for (unsigned i = 0; i < r->pre_words.size(); ++i) {
            WrapProVerb(&r->pre_words[i]);
        }
        for (unsigned i = 0; i < r->main_words.size(); ++i) {
            WrapProVerb(&r->main_words[i]);
        }
    }

    return VSS_OK;
}

VerbSayStatus VerbSayer::GetAllSayOptions(
        const VerbWithContext& vwc, size_t max_num_results,
        vector<VerbSayResult>* rr) const {
    // Check for unset fields.
    VerbSayStatus err;
    if (vwc.HasUnsetFields()) {
        return VSS_ERR_HAS_UNSET_FIELDS;
    }

    // Check pro-verb vs relative clausiness.
    if (vwc.verb().is_pro_verb() && vwc.relative_cont() == RC_ZERO) {
        return VSS_INVALID_RELATIVE_PRO_VERB_CONFLICT;
    }

    if (vwc.verb().verb_form() != VF_FINITE) {
        // Check splitness vs finiteness.
        if (vwc.is_split()) {
            return VSS_INVALID_CAN_ONLY_SPLIT_FINITE;
        }

        const Modality& m = vwc.verb().modality();
        if (m.flavor() != MF_INDICATIVE || m.is_cond()) {
            return VSS_INVALID_IF_NON_FINITE_MODALITY_MUST_BE_INDICATIVE;
        }
    }

    // Polarity, expected polarity -> Whether.
    Whether whether;
    if (vwc.verb().polarity().tf()) {
        if (vwc.verb().polarity().is_contrary().is_true()) {
            whether = WHETHER_EMPH;  // "No, she *does* write."
        } else if (vwc.is_split()) {
            whether = WHETHER_EMPH;  // "Does she write?"
        } else if (vwc.verb().is_pro_verb()) {
            whether = WHETHER_EMPH;  // "Yes, she does."
        } else {
            whether = WHETHER_YES;   // "She writes."
        }
    } else {
        whether = WHETHER_NO;      // "No, she doesn't write."
    }

    // Voice -> surface voice.
    SurfaceVoice voice;
    conv_.GetSurfaceVoice(vwc.voice(), &voice);

    // Aspect -> surface aspect.
    SurfaceAspect aspect;
    conv_.GetSurfaceAspect(vwc.verb().aspect(), &aspect);

    // Deep verb form, relative containment -> surface verb form.
    SurfaceVerbForm verb_form;
    err = conv_.GetSurfaceVerbForm(
            vwc.verb().verb_form(), vwc.relative_cont(), &verb_form);
    if (err != VSS_OK) {
        return err;
    }

    // Modality, deep tense -> mood, modal, surface tense.
    vector<MoodModalStense> mmts;
    err = conv_.GetMoodsModalsTenses(
                      vwc.verb().modality(), vwc.verb().tense(), &mmts);
    if (err != VSS_OK) {
        return err;
    }

    bool split_inf = vwc.split_inf().is_true();

    for (unsigned i = 0; i < mmts.size(); ++i) {
        if (i == max_num_results) {
            break;
        }

        const MoodModalStense& mmt = mmts[i];

        bool use_were_sbj = vwc.sbj_handling() == SH_WERE_SBJ;

        SurfaceVerb sv;
        sv.Init(vwc.verb().lemma(), whether, mmt.stense, aspect, mmt.modal,
                mmt.mood, verb_form, voice, vwc.conj(), split_inf,
                use_were_sbj);

        vector<string> ss;
        err = surface_.Say(sv, &ss);
        if (err != VSS_OK) {
            return err;
        }

/*
        string _;
        vwc.ToJSON(&_);
        printf("about to slice\n");
        printf("%s\n", _.c_str());
*/

        VerbSayResult r;
        err = SliceVerbWords(
                ss, vwc.is_split(), vwc.verb().is_pro_verb(),
                vwc.contract_not().is_true(), &r);
        if (err != VSS_OK) {
            return err;
        }
        rr->emplace_back(r);
    }

    return VSS_OK;
}

VerbSayStatus VerbSayer::Say(
        const VerbWithContext& vwc, VerbSayResult* r) const {
    vector<VerbSayResult> rr;
    VerbSayStatus err = GetAllSayOptions(vwc, 1, &rr);
    if (rr.size()) {
        *r = rr[0];
    }
    return err;
}

bool VerbSayer::IsValid(const VerbWithContext& vwc) const {
    // The only way to check validity is to go through the motions of generating
    // the words.
    VerbSayResult r;
    return Say(vwc, &r) == VSS_OK;
}
