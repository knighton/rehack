#include "surface_verb_converter.h"

#include <cassert>

#include "cc/base/table_util.h"

// -----------------------------------------------------------------------------

ModalAndIsPerf::ModalAndIsPerf() {
}

ModalAndIsPerf::ModalAndIsPerf(const string& m, bool ip) {
    modal = m;
    is_perf = ip;
}

// -----------------------------------------------------------------------------

bool ModalPastTenseConverter::Init(const string& text) {
    StringTable table;
    if (!TableUtil::ParseStringTable(text, &table)) {
        return false;
    }

    modal2past_.clear();
    for (unsigned i = 0; i < table.row_keys().size(); ++i) {
        const string& modal = table.row_keys()[i];
        ModalAndIsPerf past;
        table.Get(modal, "PAST", &past.modal);
        string tmp;
        table.Get(modal, "HAVE", &tmp);
        if (tmp == "-") {
            past.is_perf = false;
        } else if (tmp == "have") {
            past.is_perf = true;
        } else {
            return false;
        }
        modal2past_[modal] = past;
    }
    return true;
}

bool ModalPastTenseConverter::GetPastForm(
        const string& modal, string* past_modal, bool* past_is_perf) const {
    map<string, ModalAndIsPerf>::const_iterator it = modal2past_.find(modal);
    if (it == modal2past_.end()) {
        return false;
    }

    *past_modal = it->second.modal;
    *past_is_perf = it->second.is_perf;
    return true;
}

// -----------------------------------------------------------------------------

bool SurfaceVerbConverter::Init(const string& modal_past_tense_table_text) {
    mood2stenses_ = {
        {
            MOOD_IND,
            vector<SurfaceTense>({ST_PAST, ST_NONPAST})
        },
        {
            MOOD_IMP,
            vector<SurfaceTense>({ST_NONPAST})
        },
        {
            MOOD_SBJ_IMP,
            vector<SurfaceTense>({ST_SBJ_PRES})
        },
        {
            MOOD_SBJ_CF,
            vector<SurfaceTense>({ST_SBJ_PAST, ST_SBJ_FUT})
        }
    };

    return mptc_.Init(modal_past_tense_table_text);
}

bool SurfaceVerbConverter::IsTenseOkForMood(
        SurfaceTense tense, Mood mood) const {
    map<Mood, vector<SurfaceTense> >::const_iterator it =
            mood2stenses_.find(mood);
    const vector<SurfaceTense>& tt = it->second;
    for (unsigned i = 0; i < tt.size(); ++i) {  // n=3 at max.
        if (tense == tt[i]) {
            return true;
        }
    }
    return false;
}

VerbSayStatus SurfaceVerbConverter::MightBeValid(const SurfaceVerb& v) const {
    // Modality is handled in English as either a mood or a modal verb (the
    // other being set to the default in this surface structure).
    if (v.modal().size() && v.mood() != MOOD_IND) {
        return VSS_INVALID_CANT_HAVE_BOTH_MODALS_AND_NON_IND;
    }

    // Imperatives.
    if (v.mood() == MOOD_IMP) {
        if (!(v.conj() == CONJ_S2 || v.conj() == CONJ_P2)) {
            return VSS_INVALID_IMPERATIVES_ARE_2ND_PERSON;
        }
    }

    // Check that the partially mood-specific surface tense works with the mood
    // selected.
    if (!IsTenseOkForMood(v.tense(), v.mood())) {
        return VSS_SURFACE_TENSE_NOT_OK_WITH_MOOD;
    }

    // Non-finite verbs.
    if (!v.IsFinite()) {
        if (v.modal().size() || v.mood() != MOOD_IND) {
            return VSS_INVALID_NON_FINITES_CANT_HAVE_MOODS_OR_MODALS;
        }
    }

    return VSS_OK;
}

bool SurfaceVerbConverter::HandleModalPastTense(
        const string& modal, SurfaceTense stense, bool is_perf,
        string* use_modal, bool* use_perf) const {
    if (stense != ST_PAST || modal.empty()) {
        *use_modal = modal;
        *use_perf = is_perf;
        return true;
    }

    if (!mptc_.GetPastForm(modal, use_modal, use_perf)) {
        return false;
    }

    if (is_perf) {
        *use_perf = true;
    }

    return true;
}

// -----------------------------------------------------------------------------
