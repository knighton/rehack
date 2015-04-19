#include "verb_parser.h"

#include <string>

#include "cc/base/combinatorics.h"
#include "cc/base/file.h"
#include "cc/base/logging.h"
#include "cc/base/warning.h"
#include "cc/format/json.h"
#include "cc/core/ling/verb/verb_with_context.h"

// -----------------------------------------------------------------------------

void LookupTableGenerationRound::Init(
        const vector<uint8_t>& global_num_options_per_field,
        const map<FlatVWCField, vector<uint8_t> >& options_overrides) {
    // Initialize to globals.
    num_options_per_field_.resize(global_num_options_per_field.size());
    for (auto i = 0u; i < global_num_options_per_field.size(); ++i) {
        num_options_per_field_[i] = global_num_options_per_field[i];
    }
    option2global_option_.resize(global_num_options_per_field.size());

    // Then set my overrides.
    for (auto& it : options_overrides) {
        FlatVWCField field = it.first;
        const vector<uint8_t>& options = it.second;
        num_options_per_field_[field] = static_cast<uint8_t>(options.size());
        for (uint8_t i = 0; i < options.size(); ++i) {
            const uint8_t& option_value = options[i];
            option2global_option_[field][i] = option_value;
        }
    }
}

// -----------------------------------------------------------------------------

void LookupTableConfig::Init(
        const vector<uint8_t>& global_num_options_per_field,
        const vector<string>& lemmas, const vector<uint8_t>& is_pro_verbs) {
    // Set.
    global_num_options_per_field_ = global_num_options_per_field;
    lemmas_ = lemmas;

    // Restrictions on finite verbs.
    map<FlatVWCField, vector<uint8_t> > finite_overrides = {
        {FLAT_VERB_FORM,   vector<uint8_t>({VF_FINITE})},
        {FLAT_IS_PRO_VERB, is_pro_verbs}
    };

    LookupTableGenerationRound finites;
    finites.Init(global_num_options_per_field, finite_overrides);
    rounds_.emplace_back(finites);

    // Restrictions on non-finite verbs.
    map<FlatVWCField, vector<uint8_t> > nonfinite_overrides = {
        {
            FLAT_FLAVOR,
            vector<uint8_t>({
                MF_INDICATIVE
            })
        },
        {
            FLAT_VERB_FORM,
            vector<uint8_t>({
                VF_BARE_INF,
                VF_TO_INF,
                VF_GERUND,
                VF_SUBJLESS_GERUND
            })
        },
        {
            FLAT_IS_SPLIT,
            vector<uint8_t>({
                false
            }),
        },
        {
            FLAT_IS_PRO_VERB,
            is_pro_verbs
        }
    };

    LookupTableGenerationRound nonfinites;
    nonfinites.Init(global_num_options_per_field, nonfinite_overrides);
    rounds_.emplace_back(nonfinites);
}

// -----------------------------------------------------------------------------

void LookupTable::Generate(
        const LookupTableConfig& cfg, const VerbSayer* sayer) {
    map<string, vector<vector<uint8_t> > > key2tuples;
    map<VerbSayStatus, size_t> err2count;
    unsigned count = 0;
    for (size_t i = 0; i < cfg.rounds().size(); ++i) {
        const LookupTableGenerationRound& round = cfg.rounds()[i];
        vector<uint8_t> selected_options;
        while (Combinatorics::NextChooseOneFromEach(
                round.num_options_per_field(), &selected_options)) {
            // Logging.
            if (count % 1000000 == 0) {
                DEBUG("LookupTable: Trying all VWC possibilities, currently at "
                      "#%u\n", count);
            }

            // Create the flat vector, translating options if necessary.
            vector<uint8_t> translated_selected_options;
            for (size_t j = 0; j < selected_options.size(); ++j) {
                const map<uint8_t, uint8_t>& m = \
                    round.option2global_option()[j];
                uint8_t value;
                if (m.size()) {
                    value = m.find(selected_options[j])->second;
                } else {
                    value = selected_options[j];
                }
                translated_selected_options.emplace_back(value);
            }

            // Create the VerbWithContext from the flat vector.
            VerbWithContext vwc;
            vwc.InitFromVector(translated_selected_options, cfg.lemmas());

            // Render the verb to words.
            vector<VerbSayResult> rr;
            VerbSayStatus err = sayer->GetAllSayOptions(vwc, ~0ul, &rr);
            ++err2count[err];

            // Save each rendering.
            for (auto& r : rr) {
                string key;
                r.ToKey(&key);
                key2tuples[key].emplace_back(translated_selected_options);
            }
            ++count;
        }
    }

    DEBUG("LookupTable: About to collapse the generated VWC tuples.\n");

    // For each unique rendered verb words,
    key2vwcs_.clear();
    for (auto& it : key2tuples) {
        const string& key = it.first;
        vector<vector<uint8_t> >& tuples = it.second;

        // Collapse the tuples.
        Combinatorics::CollapseToWildcards(
                cfg.global_num_options_per_field(), &tuples);

        // Convert flat tuples to VerbWithContexts.
        for (auto& tuple : tuples) {
            VerbWithContext vwc;
            vwc.InitFromVector(tuple, cfg.lemmas());
            key2vwcs_[key].emplace_back(vwc);
        }
    }

    // Logging.
    INFO("Lookup table generation results:");
    for (auto& it : err2count) {
        INFO("vss %u:\t%zu\n", it.first, it.second);
    }

    map<size_t, vector<string> > count2keys;
    for (auto& it : key2vwcs_) {
        const string& key = it.first;
        size_t vwc_count = it.second.size();
        count2keys[vwc_count].emplace_back(key);
    }

/*
    for (auto& it : count2keys) {
        size_t vwc_count = it.first;
        for (auto& s : it.second) {
            DEBUG("* %zu\t%s\n", vwc_count, s.c_str());
        }
    }
*/
}

void LookupTable::ToJSON(string* s) const {
    map<string, string> key2vwcs;
    for (auto& it : key2vwcs_) {
        const string& key = it.first;
        const vector<VerbWithContext>& vwcs = it.second;

        vector<string> v;
        for (size_t i = 0; i < vwcs.size(); ++i) {
            string tmp;
            vwcs[i].ToJSON(&tmp);
            v.emplace_back(tmp);
        }

        string vwcs_s;
        json::VectorToJSON(v, json::OBJECT, &vwcs_s);

        key2vwcs[key] = vwcs_s;
    }

    string key2vwcs_s;
    json::MapToJSON(key2vwcs, json::OBJECT, &key2vwcs_s);

    json::ToJSON(s,
        json::OBJECT, "key2vwcs", key2vwcs_s
    );
}

bool LookupTable::FromJSON(const string& s) {
    string key2vwcs_s;
    if (!json::FromJSON(s,
            json::OBJECT, "key2vwcs", &key2vwcs_s
    )) {
        return false;
    }

    map<string, string> key2obj;
    if (!json::MapFromJSON(key2vwcs_s, json::OBJECT, &key2obj)) {
        return false;
    }

    key2vwcs_.clear();
    for (auto& it : key2obj) {
        const string& key = it.first;
        const string& obj_s = it.second;

        vector<string> vwcs_ss;
        if (!json::VectorFromJSON(obj_s, json::OBJECT, &vwcs_ss)) {
            return false;
        }

        for (size_t i = 0; i < vwcs_ss.size(); ++i) {
            VerbWithContext vwc;
            if (!vwc.FromJSON(vwcs_ss[i])) {
                return false;
            }
            key2vwcs_[key].emplace_back(vwc);
        }
    }

    return true;
}

void LookupTable::AppendMatches(
        const string& key, vector<VerbWithContext>* rr) const {
    auto it = key2vwcs_.find(key);
    if (it == key2vwcs_.end()) {
        return;
    }

    for (size_t i = 0; i < it->second.size(); ++i) {
        rr->emplace_back(it->second[i]);
    }
}

// -----------------------------------------------------------------------------

#define U8(a) static_cast<uint8_t>(a)

void VerbParser::GenerateConfig(const VerbSayer* sayer) {
    vector<uint8_t> global_num_options_per_field = {
        U8(1),                    //  0 string vwc.verb().lemma()
        U8(2),                    //  1 bool vwc.verb().polarity().tf()
        U8(3),                    //  2 throol vwc.verb().polarity().is_contrary()
        U8(T_NUM_TENSES),         //  3 Tense vwc.verb().tense()
        U8(2),                    //  4 bool vwc.verb().aspect().is_perf()
        U8(2),                    //  5 bool vwc.verb().aspect().is_prog()
        U8(MF_NUM_FLAVORS),       //  6 ModalFlavor vwc.verb().modality().flavor()
        U8(2),                    //  7 bool vwc.verb().modality().is_cond()
        U8(VF_NUM_VERB_FORMS),    //  8 VerbForm vwc.verb().verb_form()
        U8(2),                    //  9 bool vwc.verb().is_pro_verb()
        U8(V_NUM_VOICES),         // 10 Voice vwc.voice()
        U8(CONJ_NUM_CONJS),       // 11 Conjugation vwc.conj()
        U8(2),                    // 12 bool vwc.is_split()
        U8(RC_NUM_REL_CONTS),     // 13 RelativeContainment vwc.relative_cont()
        U8(3),                    // 14 throol vwc.contract_not()
        U8(3),                    // 15 throol vwc.split_inf()
        U8(SH_NUM_SBJ_HANDLINGS)  // 16 SubjunctiveHandling vwc.sbj_handling()
    };

    LookupTableConfig cfg;
    vector<string> lemmas;
    vector<uint8_t> is_pro_verbs;

    lemmas = vector<string>({"be"});
    is_pro_verbs = vector<uint8_t>({false, true});
    cfg.Init(global_num_options_per_field, lemmas, is_pro_verbs);
    to_be_.Generate(cfg, sayer);

    lemmas = vector<string>({"see"});
    is_pro_verbs = vector<uint8_t>({true});
    cfg.Init(global_num_options_per_field, lemmas, is_pro_verbs);
    pro_verbs_.Generate(cfg, sayer);

    lemmas = vector<string>({"<ints>"});
    is_pro_verbs = vector<uint8_t>({false});
    cfg.Init(global_num_options_per_field, lemmas, is_pro_verbs);
    fir_.Generate(cfg, sayer);
}

#undef U8

static void DelemmatizeVerb(const VerbSayResult& vsr, VerbSayResult* r) {
    r->pre_words = vsr.pre_words;
    r->main_words = vsr.main_words;
    r->main_words[r->main_words.size() - 1].clear();
}

bool VerbParser::Init(
        const Conjugator* conjugator, const string& verb_parse_f,
        const VerbSayer* sayer_or_null) {
    assert(conjugator);
    conjugator_ = conjugator;

    FILE* f = fopen(verb_parse_f.c_str(), "rb");
    if (f) {
        INFO("[VerbParser] Config file [%s] exists, about to load.\n",
             verb_parse_f.c_str());

        string s;
        if (!File::FileToString(verb_parse_f, &s)) {
            ERROR("Could not load file.\n");
            return false;
        }

        INFO("LookupTable] Parsing from JSON (%zu bytes).\n", s.size());

        if (!FromJSON(s)) {
            ERROR("JSON parsing failed.\n");
            return false;
        }
    } else {
        INFO("[VerbParser] Config file [%s] does not exist, about to "
             "regenerate (should take about a minute).\n",
             verb_parse_f.c_str());
        assert(sayer_or_null);
        GenerateConfig(sayer_or_null);
        string s;
        ToJSON(&s);
        assert(File::StringToFile(s, verb_parse_f));
    }

    INFO("[VerbParser] Loaded %zu 'to be' VWCs, %zu pro-verb VWCs, and %zu "
         "generic field index-replacing VWCs.\n", to_be_.key2vwcs().size(),
         pro_verbs_.key2vwcs().size(), fir_.key2vwcs().size());

    deverbed_keys_set_.clear();
    for (auto& it : fir_.key2vwcs()) {
        const string& key = it.first;
        VerbSayResult vsr;
        assert(vsr.FromKey(key));
        UNUSED(key);  // Clang doesn't see code inside asserts.
        VerbSayResult dvsr;
        DelemmatizeVerb(vsr, &dvsr);
        string dkey;
        dvsr.ToKey(&dkey);
        deverbed_keys_set_.insert(dkey);
    }

    return true;
}

void VerbParser::ToJSON(string* s) const {
    string be_s;
    to_be_.ToJSON(&be_s);

    string pro_verb_s;
    pro_verbs_.ToJSON(&pro_verb_s);

    string fir_s;
    fir_.ToJSON(&fir_s);

    json::ToJSON(s,
        json::OBJECT, "to_be",     be_s,
        json::OBJECT, "pro_verbs", pro_verb_s,
        json::OBJECT, "fir",       fir_s
    );
}

bool VerbParser::FromJSON(const string& s) {
    string be_s;
    string pro_verb_s;
    string fir_s;
    if (!json::FromJSON(s,
        json::OBJECT, "to_be",     &be_s,
        json::OBJECT, "pro_verbs", &pro_verb_s,
        json::OBJECT, "fir",       &fir_s
    )) {
        return false;
    }

    if (!to_be_.FromJSON(be_s)) {
        return false;
    }

    if (!pro_verbs_.FromJSON(pro_verb_s)) {
        return false;
    }

    if (!fir_.FromJSON(fir_s)) {
        return false;
    }

    return true;
}

static bool IsLastWordOk(const string& s) {
    if (s == "<ints>") {
        return true;
    }

    for (char c : s) {
        if (!('a' <= c && c <= 'z')) {
            return false;
        }
    }

    return true;
}

void VerbParser::AppendFirMatches(
        const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const {
    // It must have words to the right of the subject.  If not, it could be a
    // pro-verb or an instance of "to be", but not this.
    if (!vsr.main_words.size()) {
        return;
    }

    // Lemma-specific conjugated word must be verified to look like a verb, as
    // it isn't implicitly checked by the lookup table generation like the
    // others.
    const string& last = vsr.main_words[vsr.main_words.size() - 1];
    if (!IsLastWordOk(last)) {
        return;
    }

    // The lemma-agnostic rest of the verb must be known.
    VerbSayResult deverbed;
    DelemmatizeVerb(vsr, &deverbed);
    string deverbed_key;
    deverbed.ToKey(&deverbed_key);
    if (deverbed_keys_set_.find(deverbed_key) == deverbed_keys_set_.end()) {
        return;
    }

    // Decode what the lemma-specific word means.
    vector<LemmaAndIndex> lemmas_indexes;
    conjugator_->IdentifyWord(last, true, &lemmas_indexes);

    // For each decoding,
    for (auto& li : lemmas_indexes) {
          // Look up the field index-replacing form in the table.
        deverbed.main_words[deverbed.main_words.size() - 1] =
                std::to_string(li.index);
        string key;
        deverbed.ToKey(&key);
        vector<VerbWithContext> sub_vwcs;
        fir_.AppendMatches(key, &sub_vwcs);

        // Put our decoded lemma into the results found.
        for (auto& vwc : sub_vwcs) {
            vwc.set_lemma(li.lemma);
            vwcs->emplace_back(vwc);
        }
    }
}

void VerbParser::Parse(
        const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const {
    vwcs->clear();

    string key;
    vsr.ToKey(&key);

    DEBUG("[VerbParser::Parse] key = [%s].\n", key.c_str());

    to_be_.AppendMatches(key, vwcs);

    pro_verbs_.AppendMatches(key, vwcs);

    AppendFirMatches(vsr, vwcs);
}

// -----------------------------------------------------------------------------
