#ifndef CC_CORE_LING_VERB_VERB_MANAGER_H_
#define CC_CORE_LING_VERB_VERB_MANAGER_H_

#include "cc/core/ling/verb/internal/conjugation/conjugator.h"
#include "cc/core/ling/verb/internal/parsing/verb_parser.h"
#include "cc/core/ling/verb/internal/saying/verb_sayer.h"
#include "cc/core/ling/verb/verb_say_result.h"
#include "cc/core/ling/verb/verb_say_status.h"
#include "cc/core/ling/verb/verb_with_context.h"

class VerbManager {
  public:
    bool Init(const string& conjugations_f, const string& modal_past_tense_f,
              const string& modalities_f, const string& verb_parses_f);

    bool IsValid(const VerbWithContext& vwc) const;

    VerbSayStatus GetAllSayOptions(
        const VerbWithContext& vwc, size_t max_num_results,
        vector<VerbSayResult>* rr) const;

    VerbSayStatus Say(const VerbWithContext& vwc, VerbSayResult* r) const;

    void Parse(const VerbSayResult& vsr, vector<VerbWithContext>* vwcs) const;

  private:
    Conjugator conjugator_;
    VerbSayer sayer_;
    VerbParser parser_;
};

#endif  // CC_CORE_LING_VERB_VERB_MANAGER_H_
