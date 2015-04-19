#ifndef CC_PAN_LING_MISC_INFLECTIONS_H_
#define CC_PAN_LING_MISC_INFLECTIONS_H_

#include <map>
#include <string>
#include <vector>

#include "cc/base/enum_strings.h"
#include "cc/core/ling/misc/grammatical_numbers.h"

using std::map;
using std::string;
using std::vector;

enum Conjugation {
    CONJ_S1,
    CONJ_S2,
    CONJ_S3,
    CONJ_P1,
    CONJ_P2,
    CONJ_P3,
    CONJ_NUM_CONJS
};

extern EnumStrings<Conjugation> ConjugationStrings;

extern Conjugation CONJ_FROM_GRAM_NUM2[];

// Inflection: the single concept connecting choices of verb conjugation,
// personal pronoun, etc.

enum Inflection {
    INFL_I,
    INFL_WE,
    INFL_YOU1,
    INFL_YOU2,
    INFL_HE,
    INFL_SHE,
    INFL_IT,
    INFL_THEY1,
    INFL_ONE,
    INFL_WHO1,
    INFL_WHO2,
    INFL_THEY2,
    INFL_WHOEVER1,
    INFL_WHOEVER2,
    INFL_NUM_INFLS
};

extern EnumStrings<Inflection> InflectionStrings;

extern bool IS_INFLECTION_A_QUESTION[];

extern Conjugation BARE_PERS_PRO_CONJUGATIONS[];

enum Gender {
    GENDER_MALE,
    GENDER_FEMALE,
    GENDER_NEUTER,
    GENDER_NUM_GENDERS
};

enum Person {
    PERSON_1ST,
    PERSON_2ND,
    PERSON_3RD
};

struct PersonGroup {
    bool p1;
    bool p2;
    bool p3;

    PersonGroup(bool has_1p, bool has_2p, bool has_3p) :
                p1(has_1p), p2(has_2p), p3(has_3p) {}
};

class InflectionInfo {
  public:
    Inflection enum_value() const { return enum_value_; }
    Conjugation normal_conjugation() const { return normal_conjugation_; }
    GramNum2 number() const { return number_; }
    Gender gender() const { return gender_; }
    const vector<PersonGroup>& person_groups() const { return person_groups_; }

    void Init(Inflection i, Conjugation c, GramNum2 n, Gender g,
              const vector<PersonGroup>& pgs);

    Conjugation DecideConjugation(bool is_bare_pers_pro) const;

  private:
    // Personal pronoun, augmented with number for uniform conjugation.
    //
    // Conflates person, number, sometimes gender, and a little bit more (eg,
    // WHO vs WHOEVER, THEY1 vs ONE vs HE, etc.).
    Inflection enum_value_;

    // Form used when conjugating verbs (3 persons x 2 numbers).
    Conjugation normal_conjugation_;

    // Number: singular or plural.  Note: this is about the count of the
    // underlying things in question, not grammatical number.
    GramNum2 number_;

    // Gender.
    Gender gender_;

    // List of possible groupings of persons it could refer to.
    vector<PersonGroup> person_groups_;
};

class InflectionManager {
  public:
    const vector<InflectionInfo>& infos() const { return infos_; }

    bool Init(const string& inflections_f);

  private:
    bool NumberFromStr(const string& s, GramNum2* n) const;
    bool GenderFromStr(const string& s, Gender* g) const;
    bool PersonGroupsFromStr(const string& s, vector<PersonGroup>* pgs) const;

    vector<InflectionInfo> infos_;

    map<string, GramNum2> s2number_;
    map<string, Gender> s2gender_;
    map<string, vector<PersonGroup> > s2person_;
};

#endif  // CC_PAN_LING_MISC_INFLECTIONS_H_
