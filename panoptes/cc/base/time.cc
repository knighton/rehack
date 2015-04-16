#include "time.h"

#include <cassert>
#include <chrono>
#include <ctime>

#include "cc/base/string_util.h"

using std::chrono::duration_cast;
using std::chrono::system_clock;

void Time::PrettyTime(string* s) {
    system_clock::time_point now = system_clock::now();
    system_clock::duration tp = now.time_since_epoch();
    time_t tt = system_clock::to_time_t(now);
    tm t = *localtime(&tt);

    double d = tp.count() % system_clock::duration::period::den;
    d /= system_clock::duration::period::den;
    assert(system_clock::duration::period::num == 1);

    String::SStringPrintf(
        s, "%04d-%02d-%02d %02d:%02d:%02d.%03d", t.tm_year + 1900, t.tm_mon + 1,
        t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, (int)(d * 1000));
}

uint64_t Time::MicrosSinceEpoch() {
    return (uint64_t)duration_cast<std::chrono::microseconds>
        (system_clock::now().time_since_epoch()).count();
}
