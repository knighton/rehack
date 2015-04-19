#include "verb_manager.h"

bool VerbManager::Init(
        const string& conjugations_f, const string& modal_past_tense_f,
        const string& modalities_f, const string& verb_parses_f) {
    if (!conjugator_.InitFromFile(conjugations_f)) {
        return false;
    }

    if (!sayer_.Init(&conjugator_, modalities_f, modal_past_tense_f)) {
        return false;
    }

    if (!parser_.Init(&conjugator_, verb_parses_f, &sayer_)) {
        return false;
    }

    return true;
}

bool VerbManager::IsValid(const VerbWithContext& vwc) const {
    return sayer_.IsValid(vwc);
}

VerbSayStatus VerbManager::GetAllSayOptions(
        const VerbWithContext& vwc, size_t max_num_results,
        vector<VerbSayResult>* rr) const {
    return sayer_.GetAllSayOptions(vwc, max_num_results, rr);
}

VerbSayStatus VerbManager::Say(
        const VerbWithContext& vwc, VerbSayResult* r) const {
    return sayer_.Say(vwc, r);
}

void VerbManager::Parse(
        const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const {
    printf("VerbManager::Parse()\n");
    vsr.Dump();
    parser_.Parse(vsr, vwcs);
}
