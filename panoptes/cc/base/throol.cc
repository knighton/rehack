#include "throol.h"

#include <cassert>

throol::throol(bool could_be_true, bool could_be_false) {
    assert(could_be_true || could_be_false);
    if (could_be_true && could_be_false) {
        value_ = 2;
    } else if (could_be_true) {
        value_ = 1;
    } else {
        value_ = 0;
    }
}

bool throol::is_true() const {
    assert(0 <= value_ && value_ <= 2);
    return value_ == 1;
}

bool throol::is_false() const {
    assert(0 <= value_ && value_ <= 2);
    return value_ == 0;
}

bool throol::is_unknown() const {
    assert(0 <= value_ && value_ <= 2);
    return value_ == 2;
}

const char* throol::ToStr() const {
    switch (value_) {
    case 0:
        return "FALSE";
    case 1:
        return "TRUE";
    case 2:
        return "UNKNOWN";
    default:
        assert(false);
    }
}

bool throol::FromStr(const string& s) {
    if (s == "FALSE") {
        value_ = 0;
    } else if (s == "TRUE") {
        value_ = 1;
    } else if (s == "UNKNOWN") {
        value_ = 2;
    } else {
        return false;
    }
    return true;
}
