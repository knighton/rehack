#ifndef CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_CONVERTER_H_
#define CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_CONVERTER_H_

#include <map>
#include <string>
#include <vector>

#include "cc/core/ling/verb/internal/surface/surface_verb.h"
#include "cc/core/ling/verb/verb_say_status.h"

using std::map;
using std::string;
using std::vector;

struct ModalAndIsPerf {
    string modal;
    bool is_perf;

    ModalAndIsPerf();
    ModalAndIsPerf(const string& m, bool ip);
};

class ModalPastTenseConverter {
  public:
    bool Init(const string& text);

    bool GetPastForm(const string& modal, string* past_modal,
                                      bool* past_is_perf) const;

  private:
    map<string, ModalAndIsPerf> modal2past_;
};

class SurfaceVerbConverter {
  public:
    bool Init(const string& text);

    bool IsTenseOkForMood(SurfaceTense tense, Mood mood) const;

    VerbSayStatus MightBeValid(const SurfaceVerb& v) const;

    bool HandleModalPastTense(
        const string& modal, SurfaceTense stense, bool is_perf,
        string* use_modal, bool* use_perf) const;

  private:
    map<Mood, vector<SurfaceTense> > mood2stenses_;
    ModalPastTenseConverter mptc_;
};

#endif  // CC_VERB_INTERNAL_SURFACE_SURFACE_VERB_CONVERTER_H_
