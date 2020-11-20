#ifndef _WIN32

#include "scheduler.h"
#include "io_event.h"
#include "hook.h"

DEF_int32(co_max_recv_size, 1024 * 1024, "#1 max size for a single recv");
DEF_int32(co_max_send_size, 1024 * 1024, "#1 max size for a single send");

namespace co {



int close(sock_t fd, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    gSched->del_event(fd);
    if (ms > 0) gSched->sleep(ms);
    return ::close(fd);
    // int r;
    // while ((r = fp_close(fd)) != 0 && errno == EINTR);
    // return r;
}


int connect(sock_t fd, const void* addr, int addrlen, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    do {
        int r = fp_connect(fd, (const sockaddr*)addr, (socklen_t)addrlen);
        if (r == 0) return 0;

        if (errno == EINPROGRESS) {
            IoEvent ev(fd, EV_write);
            if (!ev.wait(ms)) return -1;

            int err, len = sizeof(err);
            r = co::getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);
            if (r != 0) return -1;
            if (err == 0) return 0;
            errno = err;
            return -1;

        } else if (errno != EINTR) {
            return -1;
        }
    } while (true);
}

int recv(sock_t fd, void* buf, int n, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    IoEvent ev(fd, EV_read);

    do {
        int r = (int) fp_recv(fd, buf, n, 0);
        if (r != -1) return r;

        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            if (!ev.wait(ms)) return -1;
        } else if (errno != EINTR) {
            return -1;
        }
    } while (true);
}

int _Recvn(sock_t fd, void* buf, int n, int ms) {
    char* s = (char*) buf;
    int remain = n;
    IoEvent ev(fd, EV_read);

    do {
        int r = (int) fp_recv(fd, s, remain, 0);
        if (r == remain) return n;
        if (r == 0) return 0;

        if (r == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                if (!ev.wait(ms)) return -1;
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            remain -= r;
            s += r;
        }
    } while (true);
}

int recvn(sock_t fd, void* buf, int n, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    char* s = (char*) buf;
    int remain = n;

    while (remain > FLG_co_max_recv_size) {
        int r = _Recvn(fd, s, FLG_co_max_recv_size, ms);
        if (r != FLG_co_max_recv_size) return r;
        remain -= FLG_co_max_recv_size;
        s += FLG_co_max_recv_size;
    }

    int r = _Recvn(fd, s, remain, ms);
    return r != remain ? r : n;
}

int recvfrom(sock_t fd, void* buf, int n, void* addr, int* addrlen, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    IoEvent ev(fd, EV_read);
    do {
        int r = (int) fp_recvfrom(fd, buf, n, 0, (sockaddr*)addr, (socklen_t*)addrlen);
        if (r != -1) return r;

        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            if (!ev.wait(ms)) return -1;
        } else if (errno != EINTR) {
            return -1;
        }
    } while (true);
}

int _Send(sock_t fd, const void* buf, int n, int ms) {
    const char* s = (const char*) buf;
    int remain = n;
    IoEvent ev(fd, EV_write);

    do {
        int r = (int) fp_send(fd, s, remain, 0);
        if (r == remain) return n;

        if (r == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                if (!ev.wait(ms)) return -1;
            } else if (errno != EINTR) {
                return -1;
            }
        } else {
            remain -= r;
            s += r;
        }
    } while (true);
}

int send(sock_t fd, const void* buf, int n, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    const char* s = (const char*) buf;
    int remain = n;

    while (remain > FLG_co_max_send_size) {
        int r = _Send(fd, s, FLG_co_max_send_size, ms);
        if (r != FLG_co_max_send_size) return r;
        remain -= FLG_co_max_send_size;
        s += FLG_co_max_send_size;
    }

    int r = _Send(fd, s, remain, ms);
    return r != remain ? r : n;
}

int sendto(sock_t fd, const void* buf, int n, const void* addr, int addrlen, int ms) {
    CHECK(gSched) << "must be called in coroutine..";
    IoEvent ev(fd, EV_write);

    do {
        int r = (int) fp_sendto(fd, buf, n, 0, (const sockaddr*)addr, (socklen_t)addrlen);
        if (r != -1) return r;

        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            if (!ev.wait(ms)) return -1;
        } else if (errno != EINTR) {
            return -1;
        }
    } while (true);
}

// a thread-safe wrapper for strerror()
const char* strerror(int err) {
    static __thread std::unordered_map<int, const char*>* kErrStr = 0;
    if (!kErrStr) kErrStr = new std::unordered_map<int, const char*>();

    if (err == ETIMEDOUT) return "timedout";
    auto& e = (*kErrStr)[err];
    if (e) return e;

    static ::Mutex mtx;
    ::MutexGuard g(mtx);
    return e = strdup(::strerror(err));
}

} // co

#endif
