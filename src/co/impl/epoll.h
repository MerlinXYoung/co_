#pragma once

#include "hook.h"
#include "co/co.h"
#include "co/atomic.h"
#include "co/log.h"



#ifdef __linux__
#include <sys/epoll.h>
#else
#include <time.h>
#include <sys/event.h>
#endif




namespace co {

enum {
    EV_read = 1,
    EV_write =2,
};
// #define CO_EV_UD_FLAG (0x1<<31)
static constexpr uint32 EV_UD_FLAG = 0x1<<31; 
static constexpr uint64 EV_UD_FLAG_BOTH = (uint64)EV_UD_FLAG<<32|EV_UD_FLAG;//0x8000000080000000;
static_assert(EV_UD_FLAG_BOTH == 0x8000000080000000);
#ifdef __linux__

class Epoll {
  public:
    Epoll();
    ~Epoll();

    bool add_event(int fd, int ev, int ud) {
        return (ev == EV_read) ? add_ev_read(fd, ud) : add_ev_write(fd, ud);
    }

    void del_event(int fd, int ev) {
        (ev == EV_read) ? del_ev_read(fd) : del_ev_write(fd);
    }

    void del_event(int fd) {
        // auto it = _ev_map.find(fd);
        // if (it != _ev_map.end()) {
        if(_ev_map[fd]){
            // _ev_map.erase(it);
            _ev_map[fd]=0;
            if (epoll_ctl(_efd, EPOLL_CTL_DEL, fd, (epoll_event*)8) != 0) {
                ELOG << "epoll del error: " << co::strerror() << ", fd: " << fd;
            }
        }
    }

    void close();

    int wait(int ms) {
        return fp_epoll_wait(_efd, _ev, 1024, ms);
    }

    const epoll_event& operator[](int i) const {
        return _ev[i];
    }

    static bool is_ev_pipe(const epoll_event& ev) {
        return ev.data.u64 == 0;
    }

    // @ev.data.u64:
    //   higher 32 bits: id of coroutine waiting for EV_read
    //   lower  32 bits: id of coroutine waiting for EV_write
    static uint64 ud(const epoll_event& ev) {
        if (ev.events & EPOLLIN) {
            return (ev.events & EPOLLOUT) ? ev.data.u64 & ~EV_UD_FLAG_BOTH : (ev.data.u64 >> 32) & ~EV_UD_FLAG;
        } else {
            return (ev.events & EPOLLOUT) ? ((ev.data.u64 & ~EV_UD_FLAG) << 32) : ev.data.u64 & ~EV_UD_FLAG_BOTH ;
        }
    }

    void signal(char c = 'x') {
        if (atomic_compare_swap(&_signaled, 0, 1) == 0) {
            int r = (int) fp_write(_fds[1], &c, 1);
            ELOG_IF(r != 1) << "pipe write error..";
        }
    }

    void handle_ev_pipe();

  private:
    bool add_ev_read(int fd, int ud);
    bool add_ev_write(int fd, int ud);
    void del_ev_read(int fd);
    void del_ev_write(int fd);

  private:
    int _efd;
    int _fds[2]; // pipe fd
    int _signaled;
    epoll_event _ev[1024];
    // std::unordered_map<int, uint64> _ev_map;
    uint64 _ev_map[CO_MAX_FD];
};

#else // kqueue
typedef struct kevent epoll_event;

class Epoll {
  public:
    Epoll();
    ~Epoll();

    bool add_event(int fd, int ev, void* ud);

    void del_event(int fd, int ev);

    void del_event(int fd);

    void close();

    int wait(int ms) {
        if (ms >= 0) {
            struct timespec ts = { ms / 1000, ms % 1000 * 1000000 };
            return fp_kevent(_kq, 0, 0, _ev, 1024, &ts);
        } else {
            return fp_kevent(_kq, 0, 0, _ev, 1024, 0);
        }
    }

    const epoll_event& operator[](int i) const {
        return _ev[i];
    }

    static bool is_ev_pipe(const epoll_event& ev) {
        return ev.udata == 0;
    }

    static void* ud(const epoll_event& ev) {
        return ev.udata;
    }

    void signal(char c = 'x') {
        if (atomic_compare_swap(&_signaled, 0, 1) == 0) {
            int r = (int) fp_write(_fds[1], &c, 1);
            ELOG_IF(r != 1) << "pipe write error..";
        }
    }

    void handle_ev_pipe();

  private:
    int _kq;
    int _fds[2]; // pipe fd
    int _signaled;
    epoll_event _ev[1024];
    // std::unordered_map<int, int> _ev_map;
    uint64 _ev_map[CO_MAX_FD];
};

#endif // kqueue


} // co
