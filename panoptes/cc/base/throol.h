#ifndef CC_BASE_THROOL_H_
#define CC_BASE_THROOL_H_

// 3-way boolean.
//
// This is a class and not an enum because I want to name its variables like
// bools, but have bool operations fail (eg, no "if (is_var)").

#include <string>

using std::string;

extern uint8_t THROOL_INVALID;

class throol {
  public:
    throol() : value_(THROOL_INVALID) {}
    throol(bool could_be_true, bool could_be_false);

    uint8_t value() const { return value_; }

    bool is_true() const;
    bool is_false() const;
    bool is_unknown() const;

    const char* ToStr() const;
    bool FromStr(const string& s);

  private:
    uint8_t value_;
};

#endif  // CC_BASE_THROOL_H_
