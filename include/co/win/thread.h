#pragma once

#ifdef _WIN32

#include "../atomic.h"
#include "../closure.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include <memory>



inline unsigned int current_thread_id() {
    static __thread unsigned int id = 0;
    if (id != 0) return id;
    return id = GetCurrentThreadId();
}

