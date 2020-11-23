#pragma once

#ifdef _WIN32
#include "win/thread.h"
#else
#include "unix/thread.h"
#endif

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Monitor{
public:
    using lock_guard_type = std::unique_lock<std::mutex>;
    Monitor(bool manual_reset=false, bool signaled=false)
        : _consumer(0), _manual_reset(manual_reset), _signaled(signaled) {
    }
    ~Monitor()=default;
    void wait() {
        lock_guard_type g(_lock);
        if (!_signaled) {
            ++_consumer;
            _cv.wait(g);
            --_consumer;
            assert(_signaled);
        }

        if (!_manual_reset && _consumer == 0) _signaled = false;
    }

    bool wait(unsigned int ms) {
        lock_guard_type g(_lock);
        if (!_signaled) {
            ++_consumer;
            auto state = _cv.wait_for(g, std::chrono::milliseconds(ms));
            --_consumer;

            if (state == std::cv_status::timeout) return false;
            assert(state == std::cv_status::no_timeout);
            assert(_signaled);
        }

        if (!_manual_reset && _consumer == 0) _signaled = false;
        return true;
    }

    void signal() {
        lock_guard_type g(_lock);
        if (!_signaled) {
            _signaled = true;
            _cv.notify_one();
        }
    }

    void reset() {
        lock_guard_type g(_lock);
        _signaled = false;
    }
private:
    std::mutex _lock;
    std::condition_variable _cv;
    int _consumer;
    const bool _manual_reset;
    bool _signaled;
};

using SyncEvent = Monitor;

class TaskSched {
  public:
    typedef std::function<void()> F;

    TaskSched();
    ~TaskSched();

    TaskSched(TaskSched&& t) {
        _p = t._p;
        t._p = 0;
    }

    // run f() once @sec seconds later
    void run_in(F&& f, int sec);

    void run_in(const F& f, int sec) {
        this->run_in(F(f), sec);
    }

    // run f() every @sec seconds
    void run_every(F&& f, int sec);

    void run_every(const F& f, int sec) {
        this->run_every(F(f), sec);
    }

    // run_daily(f, 23, 0, 0);  ->  run f() at 23:00:00 every day
    void run_daily(F&& f, int hour=0, int min=0, int sec=0);

    void run_daily(const F& f, int hour=0, int min=0, int sec=0) {
        this->run_daily(F(f), hour, min, sec);
    }

    // stop the task schedule
    void stop();

  private:
    void* _p;
    DISALLOW_COPY_AND_ASSIGN(TaskSched);
};
