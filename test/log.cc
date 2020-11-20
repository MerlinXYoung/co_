#include "co/log.h"
#include "co/time.h"

DEF_bool(perf, false, "performance testing");

bool static_log() {
    DLOG << "hello static";
    LOG << "hello again, static";
    return true;
}

bool __ = static_log();

int main(int argc, char** argv) {
    flag::init(argc, argv);
    log::init();

    if (FLG_perf) {
        // test performance by writting 100W logs
        COUT << "print 100W logs, every log is about 50 bytes";

        Timer t;
        for (int k = 0; k < 1000000; k++) {
            LOG << "hello world " << 3;
        }
        int64 write_to_cache = t.us();

        log::close();
        int64 write_to_file = t.us();

        COUT << "All logs written to cache in " << write_to_cache << " us";
        COUT << "All logs written to file in " << write_to_file << " us";

    } else {
        // usage of other logs
        TLOG << "This is DLOG (trace).. " << 23;
        DLOG << "This is DLOG (debug).. " << 23;
        LOG  << "This is LOG  (info).. " << 23;
        WLOG << "This is WLOG (warning).. " << 23;
        ELOG << "This is ELOG (error).. " << 23;
        //FLOG << "This is FLOG (fatal).. " << 23;

        LOG_TRACE("this trace for player(%ld)", 9567898);
        LOG_DEBUG("this debug for player(%ld)", 9567898);
        LOG_INFO("this info for player(%ld)", 9567898);
        LOG_WARN("this warning for player(%ld)", 9567898);
        LOG_ERROR("this error for player(%ld)", 9567898);
        // LOG_FATAL("this fatal for player(%ld)", 95678);

        LOG_TRACE_IF(1+2>3)("this debug for player(%ld)", 9567898);
        LOG_DEBUG_IF(1+2>3)("this debug for player(%ld)", 9567898);
        LOG_INFO_IF(5>4)("this info for player(%ld)", 9567898);
        LOG_WARN_IF(5>4)("this warning for player(%ld)", 9567898);
        LOG_ERROR_IF(5>4)("this error for player(%ld)", 9567898);
        LOG_FATAL_IF(5>4)("this fatal for player(%ld)", 95678); 
    }

    return 0;
}
