#pragma once

#include "log.h"

#define _SOLOG(lvl) LOGGER(lvl) << 'S' << co::sched_id() << ' '
#define _COLOG(lvl) LOGGER(lvl) << 'S' << co::sched_id() << '.' << co::coroutine_id() << ' '

#define COLOG_TRACE _COLOG(log::xx::trace)
#define COLOG_DEBUG _COLOG(log::xx::debug)
#define COLOG_INFO  _COLOG(log::xx::info)
#define COLOG_WARN  _COLOG(log::xx::warn)
#define COLOG_ERROR _COLOG(log::xx::error)
#define COLGO_FATAL FLOG << 'S' << co::sched_id() << '.' << co::coroutine_id() << ' '


#define SOLOG_TRACE _SOLOG(log::xx::trace)
#define SOLOG_DEBUG _SOLOG(log::xx::debug)
#define SOLOG_INFO  _SOLOG(log::xx::info)
#define SOLOG_WARN  _SOLOG(log::xx::warn)
#define SOLOG_ERROR _SOLOG(log::xx::error)
#define SOLGO_FATAL FLOG << 'S' << co::sched_id() << ' '

#define COLOG COLOG_TRACE
#define SOLOG SOLOG_TRACE


