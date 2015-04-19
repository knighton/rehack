#include "verb_with_context.h"

#include "cc/format/json.h"
#include "cc/core/ling/verb/internal/parsing/flat_vwc_fields.h"

EnumStrings<Voice> VoiceStrings = EnumStrings<Voice>("ACTIVE PASSIVE UNKNOWN");

EnumStrings<RelativeContainment> RelativeContainmentStrings =
    EnumStrings<RelativeContainment>("ZERO WORD NO UNKNOWN");

EnumStrings<SubjunctiveHandling> SubjunctiveHandlingStrings =
    EnumStrings<SubjunctiveHandling>("WERE_SBJ WAS_SBJ UNKNOWN");

void VerbWithContext::set_lemma(const string& lemma) {
    verb_.set_lemma(lemma);
}

void VerbWithContext::Init(
        const Verb& v, Voice voice, Conjugation conj, bool is_split,
        RelativeContainment rc, throol contract_not, throol split_inf,
        SubjunctiveHandling sh) {
    verb_.Init(v.lemma(), v.polarity(), v.tense(), v.aspect(), v.modality(),
               v.verb_form(), v.is_pro_verb());
    voice_ = voice;
    conj_ = conj;
    is_split_ = is_split;
    relative_cont_ = rc;
    contract_not_ = contract_not;
    split_inf_ = split_inf;
    sbj_handling_ = sh;
}

void VerbWithContext::InitFromVector(
        const vector<uint8_t>& v, const vector<string>& lemmas) {
    unsigned li = v[FLAT_LEMMA];
    const string& lemma = lemmas[li];

    bool tf = static_cast<bool>(v[FLAT_TF]);
    throol is_contrary = *reinterpret_cast<const throol*>(&v[FLAT_IS_CONTRARY]);
    Polarity p;
    p.Init(tf, is_contrary);

    Tense tense = static_cast<Tense>(v[FLAT_TENSE]);

    bool is_perf = static_cast<bool>(v[FLAT_IS_PERF]);
    bool is_prog = static_cast<bool>(v[FLAT_IS_PROG]);
    Aspect a;
    a.Init(is_perf, is_prog);

    ModalFlavor flavor = static_cast<ModalFlavor>(v[FLAT_FLAVOR]);
    bool is_cond = static_cast<bool>(v[FLAT_IS_COND]);
    Modality m;
    m.Init(flavor, is_cond);

    VerbForm verb_form = static_cast<VerbForm>(v[FLAT_VERB_FORM]);
    bool is_pro_verb = static_cast<bool>(v[FLAT_IS_PRO_VERB]);

    verb_.Init(lemma, p, tense, a, m, verb_form, is_pro_verb);

    voice_ = static_cast<Voice>(v[FLAT_VOICE]);
    conj_ = static_cast<Conjugation>(v[FLAT_CONJ]);
    is_split_ = static_cast<bool>(v[FLAT_IS_SPLIT]);
    relative_cont_ = static_cast<RelativeContainment>(v[FLAT_REL_CONT]);
    contract_not_ = *reinterpret_cast<const throol*>(&v[FLAT_CONTRACT_NOT]);
    split_inf_ = *reinterpret_cast<const throol*>(&v[FLAT_SPLIT_INF]);
    sbj_handling_ = static_cast<SubjunctiveHandling>(v[FLAT_SBJ_HANDLING]);
}

void VerbWithContext::InitFromVWC(
        const VerbWithContext& other, Conjugation new_conj) {
    *this = other;
    conj_ = new_conj;
}

bool VerbWithContext::IsFinite() const {
    return verb_.IsFinite();
}

void VerbWithContext::ToJSON(string* s) const {
    string verb_s;
    verb_.ToJSON(&verb_s);

    json::ToJSON(s,
        json::OBJECT, "verb",          verb_s,
        json::STR,    "voice",         VoiceStrings.GetString(voice_),
        json::STR,    "conj",          ConjugationStrings.GetString(conj_),
        json::BOOL,   "is_split",      is_split_,
        json::STR,    "relative_cont",
            RelativeContainmentStrings.GetString(relative_cont_),
        json::CHARS,  "contract_not",  contract_not_.ToStr(),
        json::CHARS,  "split_inf",     split_inf_.ToStr(),
        json::STR,    "sbj_handling",
            SubjunctiveHandlingStrings.GetString(sbj_handling_)
    );
}

bool VerbWithContext::FromJSON(const string& s) {
    string verb_s;
    string voice_s;
    string conj_s;
    string relative_cont_s;
    string contract_not_s;
    string split_inf_s;
    string sbj_handling_s;
    if (!json::FromJSON(s,
        json::OBJECT, "verb",          &verb_s,
        json::STR,    "voice",         &voice_s,
        json::STR,    "conj",          &conj_s,
        json::BOOL,   "is_split",      &is_split_,
        json::STR,    "relative_cont", &relative_cont_s,
        json::STR,    "contract_not",  &contract_not_s,
        json::STR,    "split_inf",     &split_inf_s,
        json::STR,    "sbj_handling",  &sbj_handling_s
    )) {
        return false;
    }

    if (!verb_.FromJSON(verb_s)) {
        return false;
    }

    if (!VoiceStrings.MaybeGetEnumValue(voice_s, &voice_)) {
        return false;
    }

    if (!ConjugationStrings.MaybeGetEnumValue(conj_s, &conj_)) {
        return false;
    }

    if (!RelativeContainmentStrings.MaybeGetEnumValue(
            relative_cont_s, &relative_cont_)) {
        return false;
    }

    if (!contract_not_.FromStr(contract_not_s)) {
        return false;
    }

    if (!split_inf_.FromStr(split_inf_s)) {
        return false;
    }

    if (!SubjunctiveHandlingStrings.MaybeGetEnumValue(
            sbj_handling_s, &sbj_handling_)) {
        return false;
    }

    return true;
}

bool VerbWithContext::HasUnsetFields() const {
    if (voice_ == V_NUM_VOICES) {
        return true;
    }

    if (conj_ == CONJ_NUM_CONJS) {
        return true;
    }

    if (relative_cont_ == RC_NUM_REL_CONTS) {
        return true;
    }

    if (contract_not_.is_unknown()) {
        return true;
    }

    if (split_inf_.is_unknown()) {
        return true;
    }

    if (sbj_handling_ == SH_NUM_SBJ_HANDLINGS) {
        return true;
    }

    if (verb_.HasUnsetFields()) {
        return true;
    }

    return false;
}

bool VerbWithContext::IsPossibleAtSentenceRoot() const {
    // Some non-indicative moods are banned.
    const ModalFlavor& mf = verb_.modality().flavor();
    if (mf == MF_SUBJUNCTIVE_CF || mf == MF_SUBJUNCTIVE_IMP) {
        return false;
    }

    // Verb form has to be finite.
    if (verb_.verb_form() != VF_FINITE) {
        return false;
    }

    // We can't be inside a relative clause at root.
    if (relative_cont_ != RC_NO) {
        return false;
    }

    return true;
}
