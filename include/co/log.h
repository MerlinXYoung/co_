#pragma once

#include "flag.h"
#include "fastream.h"
#include "fastring.h"
#include "atomic.h"
#include "thread.h"

#ifdef _MSC_VER
#pragma warning (disable:4722)
#endif

DEC_int32(min_log_level);

namespace ___ {
namespace log {

// log::init() must be called once at the beginning of main().
void init();

// Write all buffered logs to destination and stop the logging thread.
void close();

namespace xx {

void push_fatal_log(fastream* fs);
void push_level_log(fastream* fs, int level);

extern __thread fastream* xxLog;

enum LogLevel {
    trace =0,
    debug = 1,
    info = 2,
    warning = 3,
    error = 4,
    fatal = 5,
};
inline const char* log_level_name(int lvl) {
    static const char* _name[] = { "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};
    return _name[lvl];
}

class LevelLogSaver {
  public:
    LevelLogSaver(const char* file, unsigned line, const char* func, int level) : _level(level) {
        if (xxLog == 0) xxLog = new fastream(128);
        xxLog->clear();

        (*xxLog) << log_level_name(level)<<' ';
        xxLog->resize(19); // make room for time: TRACE 1108 18:16:08
        (*xxLog) << ' ' << current_thread_id() << ' ' << file << ':' 
            << line << '(' << func << ")]" << ' ';
    }

    ~LevelLogSaver() {
        (*xxLog) << '\n';
        push_level_log(xxLog, _level);
    }

    fastream& fs() {
        return *xxLog;
    }

  private:
    int _level;
};

class FatalLogSaver {
  public:
    FatalLogSaver(const char* file, unsigned int line, const char* func) {
        if (xxLog == 0) xxLog = new fastream(128);
        xxLog->clear();
        (*xxLog) << ' ' << current_thread_id() << ' ' << file << ':' 
            << line << '(' << func << ")]" << ' ';
    }

    ~FatalLogSaver() {
        (*xxLog) << '\n';
        push_fatal_log(xxLog);
    }

    fastream& fs() {
        return *xxLog;
    }
};

class CLogSaver {
  public:
    CLogSaver() = default;

    CLogSaver(const char* file, unsigned int line, const char* func) {
        // _fs << file << ':' << line << ']' << ' ';
        _fs << file << ':' << line << '(' << func << ")]" << ' ';
    }

    ~CLogSaver() {
        _fs << '\n';
        ::fwrite(_fs.data(), 1, _fs.size(), stderr);
    }

    fastream& fs() {
        return _fs;
    }

  private:
    fastream _fs;
};

} // namespace xx
} // namespace log
} // namespace ___

using namespace ___;

#define COUT   log::xx::CLogSaver().fs()
#define CLOG   log::xx::CLogSaver(__FILE__, __LINE__, __FUNCTION__).fs()

// DLOG  ->  DEBUG
// LOG   ->  INFO
// WLOG  ->  WARNING
// ELOG  ->  ERROR
// FLOG  ->  FATAL    A FATAL log will terminate the program.
// CHECK ->  FATAL
//
// LOG << "hello world " << 23;
// WLOG_IF(1 + 1 == 2) << "xx";
#define LOGGER(lvl) \
if (FLG_min_log_level <= lvl) log::xx::LevelLogSaver(__FILE__, __LINE__, __FUNCTION__, lvl).fs()

#define TLOG  LOGGER(log::xx::trace)
#define DLOG  LOGGER(log::xx::debug)
#define LOG   LOGGER(log::xx::info)
#define WLOG  LOGGER(log::xx::warning)
#define ELOG  LOGGER(log::xx::error)
#define _FLOG log::xx::FatalLogSaver(__FILE__, __LINE__, __FUNCTION__).fs()
#define FLOG  _FLOG << "fatal error! "

#define TLOG_IF(cond) if (cond) TLOG
#define DLOG_IF(cond) if (cond) DLOG
#define  LOG_IF(cond) if (cond) LOG
#define WLOG_IF(cond) if (cond) WLOG
#define ELOG_IF(cond) if (cond) ELOG
#define FLOG_IF(cond) if (cond) FLOG

#define LOG_LVL(lvl, fmt, ...) \
do{\
if (FLG_min_log_level <= lvl){ \
if (log::xx::xxLog == nullptr) log::xx::xxLog = new fastream(128);\
log::xx::xxLog->clear();\
sprintf(*log::xx::xxLog, "%s mmdd hh:MM:ss %u %s:%u(%s)] " fmt "\n", log_level_name(lvl), current_thread_id(), __FILE__, __LINE__, __func__, __VA_ARGS__);\
log::xx::push_level_log(log::xx::xxLog, lvl);\
}\
}while(0)

#define LOG_TRACE(fmt, ...) LOG_LVL(log::xx::trace, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_LVL(log::xx::debug, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG_LVL(log::xx::info, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG_LVL(log::xx::warning, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_LVL(log::xx::error, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) \
do{\
if (log::xx::xxLog == nullptr) log::xx::xxLog = new fastream(128);\
log::xx::xxLog->clear();\
sprintf(*log::xx::xxLog, " %u %s:%u(%s)] " fmt "\n", current_thread_id(), __FILE__, __LINE__, __func__, __VA_ARGS__);\
log::xx::push_fatal_log(log::xx::xxLog);\
}while(0)

#define LOG_TRACE_IF(cond) if (cond) LOG_TRACE
#define LOG_DEBUG_IF(cond) if (cond) LOG_DEBUG
#define LOG_INFO_IF(cond) if (cond) LOG_INFO
#define LOG_WARN_IF(cond) if (cond) LOG_WARN
#define LOG_ERROR_IF(cond) if (cond) LOG_ERROR
#define LOG_FATAL_IF(cond) if (cond) LOG_FATAL


#define CHECK(cond) \
    if (!(cond)) _FLOG << "check failed: " #cond "! "

#define CHECK_NOTNULL(p) \
    if ((p) == 0) _FLOG << "check failed: " #p " mustn't be NULL! "

#define _CHECK_OP(a, b, op) \
    for (auto _x_ = std::make_pair(a, b); !(_x_.first op _x_.second);) \
        _FLOG << "check failed: " #a " " #op " " #b ", " \
              << _x_.first << " vs " << _x_.second << "! "

#define CHECK_EQ(a, b) _CHECK_OP(a, b, ==)
#define CHECK_NE(a, b) _CHECK_OP(a, b, !=)
#define CHECK_GE(a, b) _CHECK_OP(a, b, >=)
#define CHECK_LE(a, b) _CHECK_OP(a, b, <=)
#define CHECK_GT(a, b) _CHECK_OP(a, b, >)
#define CHECK_LT(a, b) _CHECK_OP(a, b, <)

// Occasional Log.
#define XX_LOG_COUNTER_NAME(x, n) XX_LOG_COUNTER_NAME_CONCAT(x, n)
#define XX_LOG_COUNTER_NAME_CONCAT(x, n) x##n
#define XX_LOG_COUNTER XX_LOG_COUNTER_NAME(XX_log_counter_, __LINE__)

#define _LOG_EVERY_N(n, what) \
    static unsigned int XX_LOG_COUNTER = 0; \
    if (atomic_fetch_inc(&XX_LOG_COUNTER) % (n) == 0) what

#define _LOG_FIRST_N(n, what) \
    static int XX_LOG_COUNTER = 0; \
    if (XX_LOG_COUNTER < (n) && atomic_fetch_inc(&XX_LOG_COUNTER) < (n)) what

#define DLOG_EVERY_N(n) _LOG_EVERY_N(n, DLOG)
#define  LOG_EVERY_N(n) _LOG_EVERY_N(n, LOG)
#define WLOG_EVERY_N(n) _LOG_EVERY_N(n, WLOG)
#define ELOG_EVERY_N(n) _LOG_EVERY_N(n, ELOG)

#define DLOG_FIRST_N(n) _LOG_FIRST_N(n, DLOG)
#define  LOG_FIRST_N(n) _LOG_FIRST_N(n, LOG)
#define WLOG_FIRST_N(n) _LOG_FIRST_N(n, WLOG)
#define ELOG_FIRST_N(n) _LOG_FIRST_N(n, ELOG)
