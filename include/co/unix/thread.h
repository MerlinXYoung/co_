#pragma once

#ifndef _WIN32

#include "../atomic.h"
#include "../closure.h"

#include <pthread.h>
#include <assert.h>
#include <memory>


namespace xx {
unsigned int gettid(); // get current thread id
} // xx

// using current_thread_id here as glibc 2.30 already has a gettid
inline unsigned int current_thread_id() {
    static __thread unsigned int id = 0;
    if (id != 0) return id;
    return id = xx::gettid();
}

#endif
