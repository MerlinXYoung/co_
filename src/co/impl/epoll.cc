#include "epoll.h"
#include "hook.h"

namespace co {

void Epoll::handle_ev_pipe() {
    int dummy;
    while (true) {
        int r = fp_read(_fds[0], &dummy, 4);
        DLOG <<"fd:" << _fds[0] << " r:"<< r << " dummy:" << dummy;
        if (r != -1) {
            if (r < 4) break;
            continue;
        } else {
            if (errno == EWOULDBLOCK || errno == EAGAIN) break;
            if (errno == EINTR) continue;
            ELOG << "pipe read error: " << co::strerror() << ", fd: " << _fds[0];
            break;
        }
    }
    atomic_swap(&_signaled, 0);
}

inline void closesocket(sock_t& fd) {
    if (fd != -1) {
        while (fp_close(fd) != 0 && errno == EINTR);
        fd = -1;
    }
}

#ifdef __linux__
Epoll::Epoll() : _signaled(0) {
    _efd = epoll_create(1024);
    DLOG << "efd: " << _efd;
    CHECK_NE(_efd, -1) << "epoll create error: " << co::strerror();
    co::set_cloexec(_efd);

    CHECK_NE(::pipe(_fds), -1) << "create pipe error: " << co::strerror();
    DLOG << "pipe:{" << _fds[0] <<", " << _fds[1]<<"}";
    // co::set_cloexec(_fds[0]);
    // co::set_cloexec(_fds[1]);
    // co::set_nonblock(_fds[0]);
    fp_fcntl(_fds[0], F_SETFD, fp_fcntl(_fds[0], F_GETFD) | FD_CLOEXEC);
    fp_fcntl(_fds[1], F_SETFD, fp_fcntl(_fds[1], F_GETFD) | FD_CLOEXEC);
    fp_fcntl(_fds[0], F_SETFL, fp_fcntl(_fds[0], F_GETFL) | O_NONBLOCK);
    // _ev_map = (uint64*)calloc(FLG_co_max_fd, sizeof(uint64));
    bzero(_ev_map, sizeof(_ev_map));
    CHECK(this->add_ev_read(_fds[0], 0));
}

Epoll::~Epoll() {
    DLOG <<"";
    this->close();
    // if(_ev_map)
    // {
    //     free(_ev_map);
    //     _ev_map =nullptr;
    // }
}

void Epoll::close() {
    DLOG <<"efd:"<<_efd;
    co::closesocket(_efd);
    co::closesocket(_fds[0]);
    co::closesocket(_fds[1]);
}

bool Epoll::add_ev_read(int fd, int ud) {
    DLOG << "fd:"<< fd << " ud:"<< ud;
    auto& x = _ev_map[fd];
    if (x >> 32) return true; // already exists

    epoll_event event;
    event.events = x ? (EPOLLIN | EPOLLOUT | EPOLLET) : (EPOLLIN | EPOLLET);
    event.data.u64 = ((uint64)(ud|EV_UD_FLAG) << 32) | x;

    if (epoll_ctl(_efd, x ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, fd, &event) == 0) {
        x = event.data.u64;
        return true;
    }

    ELOG << "add ev read error: " << co::strerror() << ", fd: " << fd << ", co: " << ud;
    return false;
}

bool Epoll::add_ev_write(int fd, int ud) {
    DLOG << "fd:"<< fd << " ud:"<< ud;
    auto& x = _ev_map[fd];
    if ((uint32)x) return true; // already exists

    epoll_event event;
    event.events = x ? (EPOLLIN | EPOLLOUT | EPOLLET) : (EPOLLOUT | EPOLLET);
    event.data.u64 = x | (EV_UD_FLAG | ud);

    if (epoll_ctl(_efd, x ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, fd, &event) == 0) {
        x = event.data.u64;
        return true;
    }

    ELOG << "add ev write error: " << co::strerror() << ", fd: " << fd << ", co: " << ud;
    return false;
}

void Epoll::del_ev_read(int fd) {
    DLOG << "fd:"<< fd ;
    // auto it = _ev_map.find(fd);
    // if (it == _ev_map.end() || !(it->second >> 32)) return; // not exists
    auto& ud = _ev_map[fd];
    if ( !(ud>>32) ) return;

    DLOG << "fd:"<< fd << " ud:"<< (uint32)ud;
    int r;
    if (!(uint32)ud ) {
        ud =0;
        r = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, (epoll_event*)8);
    } else {
        ud = (uint32)ud;
        epoll_event event;
        event.events = EPOLLOUT | EPOLLET;
        event.data.u64 = ud;
        r = epoll_ctl(_efd, EPOLL_CTL_MOD, fd, &event);
    }

    ELOG_IF(r != 0) << "del ev read error: " << co::strerror() << ", fd: " << fd;
}

void Epoll::del_ev_write(int fd) {
    // auto it = _ev_map.find(fd);
    // if (it == _ev_map.end() || !(uint32)it->second) return; // not exists
    auto& ud = _ev_map[fd];
    if( !((uint32)ud)) return;

    DLOG << "fd:"<< fd << " ud:"<< (ud>> 32);
    int r;
    if (!(ud>> 32)) {
        ud=0;
        r = epoll_ctl(_efd, EPOLL_CTL_DEL, fd, (epoll_event*)8);
    } else {
        ud &= ((uint64)-1 << 32);
        epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.u64 = ud;
        r = epoll_ctl(_efd, EPOLL_CTL_MOD, fd, &event);
    }

    ELOG_IF(r != 0) << "del ev write error: " << co::strerror() << ", fd: " << fd;
}

#else  // kqueue
Epoll::Epoll() : _signaled(0) {
    _kq = kqueue();
    CHECK_NE(_kq, -1) << "kqueue create error: " << co::strerror();

    CHECK_NE(::pipe(_fds), -1) << "create pipe error: " << co::strerror();
    // co::set_cloexec(_fds[0]);
    // co::set_cloexec(_fds[1]);
    // co::set_nonblock(_fds[0]);
    fp_fcntl(_fds[0], F_SETFD, fp_fcntl(_fds[0], F_GETFD) | FD_CLOEXEC);
    fp_fcntl(_fds[1], F_SETFD, fp_fcntl(_fds[1], F_GETFD) | FD_CLOEXEC);
    fp_fcntl(_fds[0], F_SETFL, fp_fcntl(_fds[0], F_GETFL) | O_NONBLOCK);
    // _ev_map = calloc(co_max_fd, sizeof(uint64));
    bzero(_ev_map, sizeof(_ev_map));
    CHECK(this->add_event(_fds[0], EV_read, 0));
}

Epoll::~Epoll() {
    this->close();
    // if(_ev_map)
    // {
    //     free(_ev_map);
    //     _ev_map =nullptr;
    // }
}

void Epoll::close() {
    co::closesocket(_kq);
    co::closesocket(_fds[0]);
    co::closesocket(_fds[1]);
}

bool Epoll::add_event(int fd, int ev, void* p) {
    int& x = _ev_map[fd];
    if (x & ev) return true; // already exists

    struct kevent event;
    const int evfilt = (ev == EV_read ? EVFILT_READ : EVFILT_WRITE);
    EV_SET(&event, fd, evfilt, EV_ADD, 0, 0, p);

    if (fp_kevent(_kq, &event, 1, 0, 0, 0) == 0) {
        x |= ev;
        return true;
    }

    ELOG << "kqueue add event error: " << co::strerror() << ", fd: " << fd << ", ev: " << ev;
    return false;
}

void Epoll::del_event(int fd, int ev) {
    auto it = _ev_map.find(fd);
    if (it == _ev_map.end() || !(it->second & ev)) return;

    int& x = it->second;
    x == ev ? (void) _ev_map.erase(it) : (void) (x &= ~ev);

    struct kevent event;
    const int evfilt = (ev == EV_read ? EVFILT_READ : EVFILT_WRITE);
    EV_SET(&event, fd, evfilt, EV_DELETE, 0, 0, 0);

    if (fp_kevent(_kq, &event, 1, 0, 0, 0) != 0) {
        ELOG << "kqueue del event error: " << co::strerror() << ", fd: " << fd << ", ev: " << ev;
    }
}

void Epoll::del_event(int fd) {
    auto it = _ev_map.find(fd);
    if (it == _ev_map.end()) return;

    int ev = it->second;
    _ev_map.erase(it);

    int i = 0;
    struct kevent event[2];
    if (ev & EV_read) EV_SET(&event[i++], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    if (ev & EV_write) EV_SET(&event[i++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);

    if (fp_kevent(_kq, event, i, 0, 0, 0) != 0) {
        ELOG << "kqueue del event error: " << co::strerror() << ", fd: " << fd;
    }
}
#endif


} // co
