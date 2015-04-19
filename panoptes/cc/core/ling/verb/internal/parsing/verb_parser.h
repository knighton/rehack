#ifndef CC_CORE_LING_VERB_INTERNAL_PARSING_VERB_PARSER_H_
#define CC_CORE_LING_VERB_INTERNAL_PARSING_VERB_PARSER_H_

#include <map>
#include <string>
#include <vector>

#include "cc/core/ling/verb/internal/parsing/flat_vwc_fields.h"
#include "cc/core/ling/verb/internal/saying/verb_sayer.h"
#include "cc/core/ling/verb/verb_with_context.h"

using std::map;
using std::string;
using std::vector;

// The configuration options to generate a lot of verbs.
//
// We generate verbs in multiple rounds (finites + non-finites) because various
// combinations of options aren't valid and we shouldn't bother trying them.
class LookupTableGenerationRound {
  public:
    const vector<uint8_t>& num_options_per_field() const {
        return num_options_per_field_;
    }
    const vector<map<uint8_t, uint8_t> >& option2global_option() const {
        return option2global_option_;
    }

    void Init(const vector<uint8_t>& global_num_options_per_field,
              const map<FlatVWCField, vector<uint8_t> >& option_overrides);

  private:
    // My local idea of how many options each field has.
    vector<uint8_t> num_options_per_field_;

    // For each field, map my option selections to where they are the global
    // list of options for that field (like enum values, bools, etc), if they
    // differ from global.
    vector<map<uint8_t, uint8_t> > option2global_option_;
};

// The configuration for generating a single lookup table.
class LookupTableConfig {
  public:
    const vector<uint8_t>& global_num_options_per_field() const {
            return global_num_options_per_field_;
    }
    const vector<string>& lemmas() const { return lemmas_; }
    const vector<LookupTableGenerationRound>& rounds() const { return rounds_; }

    void Init(const vector<uint8_t>& global_num_options_per_field,
              const vector<string>& lemmas,
              const vector<uint8_t>& is_pro_verbs);

  private:
    // How many options per field.
    vector<uint8_t> global_num_options_per_field_;

    // We have to save the lemmas.  All others are zero-based integers (enums,
    // bools, and throols).
    vector<string> lemmas_;

    // A list of table generation options, the outputs from which are combined.
    vector<LookupTableGenerationRound> rounds_;
};

class LookupTable {
  public:
    const map<string, vector<VerbWithContext> >& key2vwcs() const {
        return key2vwcs_;
    }

    void Generate(const LookupTableConfig& cfg, const VerbSayer* sayer);

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

    void AppendMatches(const string& key, vector<VerbWithContext>* rr) const;

  private:
    map<string, vector<VerbWithContext> > key2vwcs_;
};

class VerbParser {
  public:
    bool Init(const Conjugator* c, const string& verb_parses_f,
              const VerbSayer* sayer);

    void ToJSON(string* s) const;
    bool FromJSON(const string& s);

    void Parse(const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const;

  private:
    void GenerateConfig(const VerbSayer* sayer);

    void AppendFirMatches(
        const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const;

    const Conjugator* conjugator_;

    set<string> deverbed_keys_set_;

    // "To be" is a weird verb.
    LookupTable to_be_;

    // Pro-verb lookup table.
    LookupTable pro_verbs_;

    // Field index-replacing table.
    LookupTable fir_;
};

#endif  // CC_CORE_LING_VERB_INTERNAL_PARSING_VERB_PARSER_H_
