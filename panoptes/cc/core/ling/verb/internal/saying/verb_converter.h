#ifndef CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_CONVERTER_H_
#define CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_CONVERTER_H_

#include <map>
#include <vector>

#include "cc/ds/table.h"
#include "cc/core/ling/verb/internal/surface/surface_verb.h"
#include "cc/core/ling/verb/verb_with_context.h"
#include "cc/core/ling/verb/verb_say_status.h"

using std::map;
using std::vector;

struct MoodAndModal {
    Mood mood;
    string modal;
};

class ModalitiesTable {
  public:
    bool Init(const string& text);
    void GetOptions(const Modality& m, vector<MoodAndModal>* rr) const;

  private:
    Table<ModalFlavor, bool, vector<MoodAndModal> > table_;
};

struct MoodModalStense {
    Mood mood;
    string modal;
    SurfaceTense stense;
};

class VerbConverter {
  public:
    bool Init(const string& modalities_f);

    void GetSurfaceVoice(Voice v, SurfaceVoice* r) const;

    void GetSurfaceAspect(const Aspect& a, SurfaceAspect* r) const;

    VerbSayStatus GetSurfaceVerbForm(
        VerbForm vf, RelativeContainment rc, SurfaceVerbForm* svf) const;

    void GetSurfaceTense(Tense t, Mood m, SurfaceTense* r) const;

    VerbSayStatus GetMoodsModalsTenses(
        const Modality& m, Tense t, vector<MoodModalStense>* rr) const;

  private:
    map<Voice, SurfaceVoice> dvoice2svoice_;
    map<VerbForm, SurfaceVerbForm> verb_form2sverb_form_;
    map<Mood, map<Tense, SurfaceTense> > mood2tense2stense_;
    ModalitiesTable modalities_table_;
};

#endif  // CC_CORE_LING_VERB_INTERNAL_SAYING_VERB_CONVERTER_H_
