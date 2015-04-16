#ifndef CC_BASE_TIME_H_
#define CC_BASE_TIME_H_

#include <string>

using std::string;

class Time {
  public:
    static void PrettyTime(string* s);
    static uint64_t MicrosSinceEpoch();
};

#endif  // CC_BASE_TIME_H_
