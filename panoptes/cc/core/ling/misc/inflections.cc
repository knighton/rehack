#include "inflections.h"

#include <cassert>

#include "cc/base/file.h"
#include "cc/base/logging.h"
#include "cc/base/table_util.h"

EnumStrings<Conjugation> ConjugationStrings = EnumStrings<Conjugation>(
    "S1 S2 S3 P1 P2 P3 UNKNOWN");

Conjugation CONJ_FROM_GRAM_NUM2[] = {
    CONJ_S3,  // GN2_SING
    CONJ_P3   // GN2_PLUR
};

EnumStrings<Inflection> InflectionStrings = EnumStrings<Inflection>(
    "I WE YOU1 YOU2 HE SHE IT THEY1 ONE WHO1 WHO2 THEY2 WHOEVER1 WHOEVER2 "
    "UNKNOWN");

bool IS_INFLECTION_A_QUESTION[] = {
    false,  // I
    false,  // We
    false,  // You1
    false,  // You2
    false,  // He
    false,  // She
    false,  // It
    false,  // They1
    false,  // One
    true,   // Who1
    true,   // Who2
    false,  // They2
    true,   // Whoever1
    true    // Whoever2
};

Conjugation BARE_PERS_PRO_CONJUGATIONS[] = {
    CONJ_S1,  // I
    CONJ_P1,  // We
    CONJ_S2,  // You1
    CONJ_P2,  // You2
    CONJ_S3,  // He
    CONJ_S3,  // She
    CONJ_S3,  // It
    CONJ_P3,  // They1
    CONJ_S3,  // One
    CONJ_S3,  // Who1
    CONJ_P3,  // Who2
    CONJ_P3,  // They2
    CONJ_S3,  // Whoever1
    CONJ_P3   // Whoever2
};

// -----------------------------------------------------------------------------

void InflectionInfo::Init(
        Inflection inf, Conjugation c, GramNum2 n, Gender g,
        const vector<PersonGroup>& pgs) {
    enum_value_ = inf;
    normal_conjugation_ = c;
    number_ = n;
    gender_ = g;
    person_groups_.clear();
    person_groups_.reserve(pgs.size());
    for (unsigned i = 0; i < pgs.size(); ++i) {
        person_groups_.emplace_back(pgs[i]);
    }
}

Conjugation InflectionInfo::DecideConjugation(bool is_bare_pers_pro) const {
    // It's a little more complicated because we allow singular they.  Eg,
    // - "someone (DECL_THEY1, not pers pro) is here"
    // - "they (DECL_THEY1, is pers pro) are here"
    if (is_bare_pers_pro && enum_value_ == INFL_THEY1) {
        return CONJ_P3;
    }

    return normal_conjugation_;
}

// -----------------------------------------------------------------------------

bool InflectionManager::NumberFromStr(const string& s, GramNum2* n) const {
    auto it = s2number_.find(s);
    if (it == s2number_.end()) {
        return false;
    }

    *n = it->second;
    return true;
}

bool InflectionManager::GenderFromStr(const string& s, Gender* g) const {
    auto it = s2gender_.find(s);
    if (it == s2gender_.end()) {
        return false;
    }

    *g = it->second;
    return true;
}

bool InflectionManager::PersonGroupsFromStr(
        const string& s, vector<PersonGroup>* pgs) const {
    auto it = s2person_.find(s);
    if (it == s2person_.end()) {
        return false;
    }

    pgs->clear(); pgs->reserve(it->second.size());
    for (size_t i = 0; i < it->second.size(); ++i) {
        pgs->emplace_back(it->second[i]);
    }
    return true;
}

bool InflectionManager::Init(const string& inflections_f) {
    s2number_ = {
        {"1", GN2_SING},
        {"2", GN2_PLUR}
    };

    s2gender_ = {
        {"-", GENDER_NUM_GENDERS},
        {"M", GENDER_MALE},
        {"F", GENDER_FEMALE},
        {"N", GENDER_NEUTER}
    };

    s2person_ = {
        {
            "-",
            vector<PersonGroup>()
        },
        {
            "1",
            vector<PersonGroup>({
                PersonGroup(1, 0, 0)
            })
        },
        {
            "2",
            vector<PersonGroup>({
                PersonGroup(0, 1, 0)
            })
        },
        {
            "3",
            vector<PersonGroup>({
                PersonGroup(0, 0, 1)
            })
        },
        {
            "1+",
            vector<PersonGroup>({
                PersonGroup(1, 0, 0),
                PersonGroup(1, 0, 1),
                PersonGroup(1, 1, 0),
                PersonGroup(1, 1, 1)
            })
        },
        {
            "2+",
            vector<PersonGroup>({
                PersonGroup(0, 1, 0),
                PersonGroup(0, 1, 1)
            })
        }
    };

    string text;
    if (!File::FileToString(inflections_f, &text)) {
            ERROR("InflectionManager: Could not load file [%s].\n",
                  inflections_f.c_str());
        return false;
    }

    vector<string> column_keys =  {"conj", "number", "gender", "person"};

    const vector<Inflection>& enum_values =
            InflectionStrings.enum_values_except_last();
    const vector<string>& enum_strings =
        InflectionStrings.strings_except_last();
    Table<Inflection, string, string> t;
    if (!TableUtil::ParseTable(
            text, enum_values, enum_strings, column_keys, column_keys, &t)) {
        ERROR("InflectionManager: Could not parse data table.\n");
        return false;
    }

    infos_.reserve(enum_values.size());
    for (size_t i = 0; i < enum_values.size(); ++i) {
        Inflection inf = enum_values[i];

        string s;

        Conjugation conj;
        assert(t.Get(inf, "conj", &s));
        if (!ConjugationStrings.MaybeGetEnumValue(s, &conj)) {
            ERROR("InflectionManager: Invalid conjugation [%s].\n", s.c_str());
            return false;
        }

        GramNum2 number;
        assert(t.Get(inf, "number", &s));
        if (!NumberFromStr(s, &number)) {
            ERROR("InflectionManager: Invalid number [%s].\n", s.c_str());
            return false;
        }

        Gender gender;
        assert(t.Get(inf, "gender", &s));
        if (!GenderFromStr(s, &gender)) {
            ERROR("InflectionManager: Invalid gender [%s].\n", s.c_str());
            return false;
        }

        vector<PersonGroup> person_groups;
        assert(t.Get(inf, "person", &s));
        if (!PersonGroupsFromStr(s, &person_groups)) {
            ERROR("InflectionManager: Invalid person [%s].\n", s.c_str());
            return false;
        }

        InflectionInfo info;
        info.Init(inf, conj, number, gender, person_groups);
        infos_.emplace_back(info);
    }

    INFO("InflectionManager: loaded %zu inflections.\n", infos_.size());

    return true;
}

// -----------------------------------------------------------------------------
