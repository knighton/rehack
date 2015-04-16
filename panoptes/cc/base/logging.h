#ifndef CC_BASE_LOGGING_H_
#define CC_BASE_LOGGING_H_

#include <gflags/gflags.h>
#include <string>

#include "cc/base/enum_strings.h"

using std::string;

DECLARE_int32(log_level);

DECLARE_bool(console_colors);

enum LogLevel {
  LOG_FATAL,
  LOG_ERROR,
  LOG_WARN,
  LOG_INFO,
  LOG_DEBUG
};

extern EnumStrings<LogLevel> LogLevelStrings;

void InitLogging(FILE* f=stderr);

void FATAL(const char* s, ...);

void ERROR(const char* s, ...);

void WARN(const char* s, ...);

void INFO(const char* s, ...);

void DEBUG(const char* s, ...);

string LogToStr(const string& s);

#endif  // CC_BASE_LOGGING_H_
