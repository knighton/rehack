#ifndef CC_PAN_LING_MISC_GRAMMATICAL_NUMBERS_H_
#define CC_PAN_LING_MISC_GRAMMATICAL_NUMBERS_H_

#include "cc/base/enum_strings.h"

enum GramNum2 {
    GN2_SING,
    GN2_PLUR
};

extern EnumStrings<GramNum2> GramNum2Strings;

enum GramNum3 {
    GN3_ZERO,
    GN3_SING,
    GN3_PLUR
};

enum GramNum5 {
    GN5_ZERO,
    GN5_SING,
    GN5_DUAL,
    GN5_FEW,
    GN5_MANY
};

#endif  // CC_PAN_LING_MISC_GRAMMATICAL_NUMBERS_H_
