#include "verb.h"

#include <cassert>

#include "cc/format/json.h"

EnumStrings<ModalFlavor> ModalFlavorStrings = EnumStrings<ModalFlavor>(
    "INDICATIVE SUBJUNCTIVE_CF "

    "DEDUCTIVE ALMOST_CERTAIN PROBABLE POSSIBLE "

    "IMPERATIVE SUBJUNCTIVE_IMP ABILITY PERMISSIVE NORMATIVE NECESSITY "

    "UNKNOWN");

EnumStrings<Tense> TenseStrings = EnumStrings<Tense>(
    "PAST PRESENT FUTURE UNKNOWN");

EnumStrings<VerbForm> VerbFormStrings = EnumStrings<VerbForm>(
    "FINITE BARE_INF TO_INF GERUND SUBJLESS_GERUND UNKNOWN");

// -----------------------------------------------------------------------------

void Polarity::Init(bool p, throol ic) {
    tf_ = p;
    is_contrary_ = ic;
}

void Polarity::ToJSON(string* s) const {
    string is_contrary_s;
    json::ToJSON(s,
        json::BOOL,  "tf",          tf_,
        json::CHARS, "is_contrary", is_contrary_.ToStr()
    );
}

bool Polarity::FromJSON(const string& s) {
    string is_contrary_s;
    if (!json::FromJSON(s,
        json::BOOL, "tf",          &tf_,
        json::STR,  "is_contrary", &is_contrary_s
    )) {
        return false;
    }

    if (!is_contrary_.FromStr(is_contrary_s)) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

void Aspect::Init(bool is_perf, bool is_prog) {
    is_perf_ = is_perf;
    is_prog_ = is_prog;
}

void Aspect::ToJSON(string* s) const {
    json::ToJSON(s,
        json::BOOL, "is_perf", is_perf_,
        json::BOOL, "is_prog", is_prog_
    );
}

bool Aspect::FromJSON(const string& s) {
    return json::FromJSON(s,
        json::BOOL, "is_perf", &is_perf_,
        json::BOOL, "is_prog", &is_prog_
    );
}

// -----------------------------------------------------------------------------

void Modality::Init(ModalFlavor f, bool ic) {
    flavor_ = f;
    is_cond_ = ic;
}

void Modality::ToJSON(string* s) const {
    json::ToJSON(s,
        json::STR,  "flavor",  ModalFlavorStrings.GetString(flavor_),
        json::BOOL, "is_cond", is_cond_
    );
}

bool Modality::FromJSON(const string& s) {
    string flavor_s;
    if (!json::FromJSON(s,
        json::STR,  "flavor",  &flavor_s,
        json::BOOL, "is_cond", &is_cond_
    )) {
        return false;
    }

    if (!ModalFlavorStrings.MaybeGetEnumValue(flavor_s, &flavor_)) {
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

void Verb::set_lemma(const string& new_lemma) {
    lemma_ = new_lemma;
}

void Verb::set_is_pro_verb(bool is_pro_verb) {
    is_pro_verb_ = is_pro_verb;
}

void Verb::Init(const string& l, const Polarity& p, Tense t, const Aspect& a,
                                const Modality& m, VerbForm vf, bool ipv) {
    lemma_ = l;
    polarity_.Init(p.tf(), p.is_contrary());
    tense_ = t;
    aspect_.Init(a.is_perf(), a.is_prog());
    modality_.Init(m.flavor(), m.is_cond());
    verb_form_ = vf;
    is_pro_verb_ = ipv;
}

bool Verb::IsFinite() const {
    return verb_form_ == VF_FINITE;
}

void Verb::ToJSON(string* s) const {
    string polarity_s;
    polarity_.ToJSON(&polarity_s);

    string aspect_s;
    aspect_.ToJSON(&aspect_s);

    string modality_s;
    modality_.ToJSON(&modality_s);

    json::ToJSON(s,
        json::STR,    "lemma",       lemma_,
        json::OBJECT, "polarity",    polarity_s,
        json::STR,    "tense",       TenseStrings.GetString(tense_),
        json::OBJECT, "aspect",      aspect_s,
        json::OBJECT, "modality",    modality_s,
        json::STR,    "verb_form",   VerbFormStrings.GetString(verb_form_),
        json::BOOL,   "is_pro_verb", is_pro_verb_
    );
}

bool Verb::FromJSON(const string& s) {
    string polarity_s;
    string tense_s;
    string aspect_s;
    string modality_s;
    string verb_form_s;
    if (!json::FromJSON(s,
        json::STR,    "lemma",       &lemma_,
        json::OBJECT, "polarity",    &polarity_s,
        json::STR,    "tense",       &tense_s,
        json::OBJECT, "aspect",      &aspect_s,
        json::OBJECT, "modality",    &modality_s,
        json::STR,    "verb_form",   &verb_form_s,
        json::BOOL,   "is_pro_verb", &is_pro_verb_
    )) {
        return false;
    }

    if (!polarity_.FromJSON(polarity_s)) {
        return false;
    }

    if (!TenseStrings.MaybeGetEnumValue(tense_s, &tense_)) {
        return false;
    }

    if (!aspect_.FromJSON(aspect_s)) {
        return false;
    }

    if (!modality_.FromJSON(modality_s)) {
        return false;
    }

    if (!VerbFormStrings.MaybeGetEnumValue(verb_form_s, &verb_form_)) {
        return false;
    }

    return true;
}

bool Verb::HasUnsetFields() const {
    if (polarity_.is_contrary().is_unknown()) {
        return true;
    }

    if (tense_ == T_NUM_TENSES) {
        return true;
    }

    if (modality_.flavor() == MF_NUM_FLAVORS) {
        return true;
    }

    if (verb_form_ == VF_NUM_VERB_FORMS) {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
