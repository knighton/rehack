#ifndef CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_SAYER_H_
#define CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_SAYER_H_

#include <string>
#include <vector>

#include "cc/core/ling/verb/internal/conjugation/conjugator.h"
#include "cc/core/ling/verb/internal/surface/surface_verb_sayer.h"
#include "cc/core/ling/verb/internal/saying/verb_converter.h"
#include "cc/core/ling/verb/verb_say_result.h"
#include "cc/core/ling/verb/verb_say_status.h"
#include "cc/core/ling/verb/verb_with_context.h"

using std::string;
using std::vector;

class VerbSayer {
  public:
    bool Init(const Conjugator* conj, const string& modalities_f,
              const string& modal_past_tense_f);

    VerbSayStatus GetAllSayOptions(
        const VerbWithContext& vwc, size_t max_num_results,
        vector<VerbSayResult>* rr) const;

    VerbSayStatus Say(const VerbWithContext& vwc, VerbSayResult* r) const;

    bool IsValid(const VerbWithContext& vwc) const;

  private:
    VerbSayStatus SliceVerbWords(
        const vector<string>& ss, bool is_split, bool is_pro_verb,
        bool contract_not, VerbSayResult* r) const;

    VerbConverter conv_;
    SurfaceVerbSayer surface_;
};

#endif  // CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_SAY_H_
