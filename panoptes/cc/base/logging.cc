#include "cc/base/logging.h"

#include <cstdio>
#include <cstdarg>

#include "cc/base/time.h"

DEFINE_int32(
    log_level, LOG_DEBUG,
    "Minimum level of criticality for a log message to be written.");

DEFINE_bool(console_colors, true, "Console-colorize logging output.");

EnumStrings<LogLevel> LogLevelStrings = EnumStrings<LogLevel>(
    "FATAL ERROR WARN INFO DEBUG");

const char* LOG_CONSOLE_COLOR_PREFIXES[] = {
        "\x1B[01;91m",  // Red
        "\x1B[01;91m",  // Red
        "\x1B[01;93m",  // Yellow
        "\x1B[01;94m",  // Blue
        "\x1B[01;92m",  // Green
};

const char* LOG_CONSOLE_COLOR_SUFFIX = "\x1B[0m";

FILE* LOG_F = NULL;

void InitLogging(FILE* f) {
    LOG_F = f;
}

void Log(LogLevel log_level, const char* fmt, va_list argptr) {
    // Quit if no output file.
    if (!LOG_F) {
        return;
    }

    // Lower is more serious.
    if (FLAGS_log_level < log_level) {
        return;
    }

    // Write meta info.
    string s;
    PrettyTime(&s);
    const string& log_level_s = LogLevelStrings.GetString(log_level);
    const char* color_prefix = FLAGS_console_colors ?
        LOG_CONSOLE_COLOR_PREFIXES[log_level] : "";
    const char* color_suffix = FLAGS_console_colors ?
        LOG_CONSOLE_COLOR_SUFFIX : "";
    fprintf(LOG_F, "[%s] [%s%s%s] ", s.c_str(), color_prefix,
            log_level_s.c_str(), color_suffix);

    // Write message.
    vfprintf(LOG_F, fmt, argptr);
}

void FATAL(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    Log(LOG_FATAL, fmt, argptr);
    va_end(argptr);
}

void ERROR(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    Log(LOG_ERROR, fmt, argptr);
    va_end(argptr);
}

void WARN(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    Log(LOG_WARN, fmt, argptr);
    va_end(argptr);
}

void INFO(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    Log(LOG_INFO, fmt, argptr);
    va_end(argptr);
}

void DEBUG(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    Log(LOG_DEBUG, fmt, argptr);
    va_end(argptr);
}

string LogToStr(const string& s) {
    if (s.empty()) {
        return "";
    } else if (s.size() < 40) {
        return s;
    } else {
        return s.substr(0, 40) + "...";
    }
}
