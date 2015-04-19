#include "verb_converter.h"

#include <cassert>

#include "cc/base/file.h"
#include "cc/base/string.h"
#include "cc/base/table_util.h"
#include "cc/base/warning.h"

// -----------------------------------------------------------------------------

static Mood MoodFromString(const string& s) {
    if (s == "NORMAL") {
        return MOOD_IND;
    } else if (s == "IMP") {
        return MOOD_IMP;
    } else if (s == "SBJ_IMP") {
        return MOOD_SBJ_IMP;
    } else if (s == "SBJ_CF") {
        return MOOD_SBJ_CF;
    } else {
        assert(!"Invalid mood.");
        return MOOD_IND;
    }
}

static void MoodAndModalFromString(const string& s, MoodAndModal* r) {
    assert(s.size());
    if ('a' <= s[0] && s[0] <= 'z') {
        r->mood = MOOD_IND;
        r->modal = s;
    } else {
        r->mood = MoodFromString(s);
        r->modal.clear();
    }
}

bool ModalitiesTable::Init(const string& text) {
    // Create rows and columns.
    vector<bool> is_conds = {false, true};
    vector<string> is_conds_strs = {"NORMAL", "CONDITIONAL"};

    // Parse the table.
    Table<ModalFlavor, bool, string> t;
    if (!TableUtil::ParseTable(
            text, ModalFlavorStrings.enum_values_except_last(),
            ModalFlavorStrings.strings_except_last(), is_conds,
            is_conds_strs, &t)) {
        return false;
    }

    // Convert string valeus to lists of moods/modals.
    vector<vector<MoodAndModal> > options;
    options.resize(t.values().size());
    for (unsigned i = 0; i < t.values().size(); ++i) {
        const string& s = t.values()[i];
        if (s == "-") {
            continue;
        }

        vector<string> tmp;
        String::Split(s, ',', &tmp);
        for (unsigned j = 0; j < tmp.size(); ++j) {
            const string& s2 = tmp[j];
            MoodAndModal mm;
            MoodAndModalFromString(s2, &mm);
            options[i].emplace_back(mm);
        }
    }

    // Create the real table.
    return table_.InitWithValues(
        ModalFlavorStrings.enum_values_except_last(), is_conds, options);
}

void ModalitiesTable::GetOptions(
        const Modality& m, vector<MoodAndModal>* rr) const {
    bool b = table_.Get(m.flavor(), m.is_cond(), rr);
    assert(b);
    UNUSED(b);  // clang++ is not seeing asserts for some reason.
}

// -----------------------------------------------------------------------------

bool VerbConverter::Init(const string& modalities_f) {
    dvoice2svoice_ = {
        {V_ACTIVE,  SV_ACTIVE},
        {V_PASSIVE, SV_PASSIVE}
    };

    verb_form2sverb_form_ = {
        {VF_FINITE,          SVF_NORMAL_FINITE},
        {VF_BARE_INF,        SVF_BARE_INF},
        {VF_TO_INF,          SVF_TO_INF},
        {VF_GERUND,          SVF_GERUND},
        {VF_SUBJLESS_GERUND, SVF_GERUND}
    };

    mood2tense2stense_ = {
        {
            MOOD_IND, {
                {T_PAST,    ST_PAST},
                {T_PRESENT, ST_NONPAST},
                {T_FUTURE,  ST_NONPAST}
            }
        },
        {
            MOOD_IMP, {
                {T_PAST,    ST_NONPAST},
                {T_PRESENT, ST_NONPAST},
                {T_FUTURE,  ST_NONPAST}
            }
        },
        {
            MOOD_SBJ_IMP, {
                {T_PAST,    ST_SBJ_PRES},
                {T_PRESENT, ST_SBJ_PRES},
                {T_FUTURE,  ST_SBJ_PRES}
            }
        },
        {
            MOOD_SBJ_CF, {
                {T_PAST,    ST_SBJ_PAST},
                {T_PRESENT, ST_SBJ_PAST},
                {T_FUTURE,  ST_SBJ_FUT}
            }
        }
    };

    string s;
    if (!File::FileToString(modalities_f, &s)) {
        return false;
    }
    return modalities_table_.Init(s);
}

void VerbConverter::GetSurfaceVoice(Voice v, SurfaceVoice* r) const {
    const map<Voice, SurfaceVoice>::const_iterator it = dvoice2svoice_.find(v);
    assert(it != dvoice2svoice_.end());
    *r = it->second;
}

void VerbConverter::GetSurfaceAspect(const Aspect& a, SurfaceAspect* r) const {
    r->Init(a.is_perf(), a.is_prog());
}

VerbSayStatus VerbConverter::GetSurfaceVerbForm(
        VerbForm vf, RelativeContainment rc, SurfaceVerbForm* svf) const {
    bool in_rel_clause = rc != RC_NO;
    bool is_finite = vf == VF_FINITE;

    // Check: relative clauses can only contain finite verbs.
    if (in_rel_clause && !is_finite) {
        return VSS_INVALID_REL_CLAUSES_CAN_ONLY_CONTAIN_FINITE;
    }

    // Handle the relative clause case.
    if (in_rel_clause) {
        switch (rc) {
        case RC_ZERO:
            *svf = SVF_ZERO_RELCLAUSE_FINITE;
            break;
        case RC_WORD:
            *svf = SVF_NORMAL_FINITE;
            break;
        case RC_NO:  // Cover all enum values.
        case RC_NUM_REL_CONTS:
        default:
            assert(false);
        }
        return VSS_OK;
    }

    // Handle the normal case.
    assert(rc == RC_NO);
    const map<VerbForm, SurfaceVerbForm>::const_iterator it =
            verb_form2sverb_form_.find(vf);
    assert(it != verb_form2sverb_form_.end());
    *svf = it->second;
    return VSS_OK;
}

void VerbConverter::GetSurfaceTense(Tense t, Mood m, SurfaceTense* r) const {
    const map<Mood, map<Tense, SurfaceTense> >::const_iterator it =
        mood2tense2stense_.find(m);
    assert(it != mood2tense2stense_.end());
    const map<Tense, SurfaceTense>::const_iterator jt = it->second.find(t);
    assert(jt != it->second.end());
    *r = jt->second;
}

VerbSayStatus VerbConverter::GetMoodsModalsTenses(
        const Modality& m, Tense t, vector<MoodModalStense>* rr) const {
    // Get what mood + modal combos go with that modality.
    vector<MoodAndModal> mms;
    modalities_table_.GetOptions(m, &mms);

    if (!mms.size()) {
        return VSS_INVALID_CONDITIONAL_FORM_OF_MODALITY_DNE;
    }

    for (unsigned i = 0; i < mms.size(); ++i) {
        const MoodAndModal& mm = mms[i];

        MoodModalStense r;
        r.mood = mm.mood;
        r.modal = mm.modal;

        // Handle exceptions.
        if (m.flavor() == MF_INDICATIVE && t == T_FUTURE) {
            r.modal = "will";
        }

        // Get surface tense.
        GetSurfaceTense(t, r.mood, &r.stense);

        rr->emplace_back(r);
    }

    return VSS_OK;
}

// -----------------------------------------------------------------------------
