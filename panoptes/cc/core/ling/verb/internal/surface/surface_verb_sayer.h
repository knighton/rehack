#ifndef CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_SAYER_H_
#define CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_SAYER_H_

// The ephemeral surface details of saying verbs.  Only used when saying verbs.
//
// For parsing, we use a lookup table and some conversion magic, so there is no
// reverse of this.  Which is the only safe way it could be, because saying
// verbs is very complicated.

#include <string>

#include "cc/core/ling/verb/internal/conjugation/conjugator.h"
#include "cc/core/ling/verb/internal/surface/surface_verb.h"
#include "cc/core/ling/verb/internal/surface/surface_verb_converter.h"
#include "cc/core/ling/verb/verb_say_status.h"

using std::string;

// A list of these make up the raw material for building conjugated verb words.
struct VerbField {
    // Lemma of the verb to conjugate.
    string lemma;

    // Field index in the flattened verb sepc for the lemma.
    //
    // Since zero means use the lemma as-is, can use it to treat the lemma as just
    // a raw word.
    unsigned field_index;

    VerbField(string l, unsigned fi) {
        lemma = l;
        field_index = fi;
    }

    void Dump() {
        printf("(lemma=%s fi=%u)", lemma.c_str(), field_index);
    }
};

class SurfaceVerbSayer {
  public:
    bool Init(const Conjugator* c, const string& modal_past_tense_f);

    // There is no IsValid(), because SurfaceVerbs are emphermerally created to
    // say Verbs.

    VerbSayStatus Say(const SurfaceVerb& v, vector<string>* rr) const;

  private:
    void SaySbjFut(const SurfaceVerb& v, vector<VerbField>* ff) const;

    void SayNormal(const SurfaceVerb& v, const ConjugationSpec& to_verb,
                                  bool use_perf, vector<VerbField>* ff) const;

    const Conjugator* conjugator_;
    SurfaceVerbConverter conv_;
};

#endif  // CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_SAYER_H_
