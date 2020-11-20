#include "co/all.h"
#include "co/colog.h"
#include <iostream>

DEF_string(ip, "127.0.0.1", "ip");
DEF_int32(port, 9988, "port");

struct Connection {
    sock_t fd;   // conn fd
    fastring ip; // peer ip
    int port;    // peer port
};
static size_t counter = 0;

void on_new_connection(void* p) {
    std::unique_ptr<Connection> conn((Connection*)p);
    sock_t fd = conn->fd;
    co::set_tcp_keepalive(fd);
    co::set_tcp_nodelay(fd);

    char buf[8] = { 0 };

    while (true) {
        int r = co::recv(fd, buf, 8);
        if (r == 0) {         /* client close the connection */
            co::close(fd);
            break;
        } else if (r == -1) { /* error */
            co::reset_tcp_socket(fd, 3000);
            break;
        } else {
            ++counter;
            COLOG << "server recv " << fastring(buf, r)<<" "<<counter;
            COLOG << "server send pong";
            r = co::send(fd, "pong", 4);
            if (r == -1) {
                COLOG << "server send error: " << co::strerror();
                co::reset_tcp_socket(fd, 3000);
                break;
            }
        }
    }
}

void server_fun() {
    sock_t fd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    co::set_reuseaddr(fd);

    sock_t connfd;
    int addrlen = sizeof(sockaddr_in);
    struct sockaddr_in addr;
    co::init_ip_addr(&addr, FLG_ip.c_str(), FLG_port);

    co::bind(fd, &addr, sizeof(addr));
    co::listen(fd, 1024);

    while (true) {
        addrlen = sizeof(sockaddr_in);
        connfd = co::accept(fd, &addr, &addrlen);
        COLOG << "accept new fd:"<<connfd;
        if (connfd == -1) continue;
        
        Connection* conn = new Connection;
        conn->fd = connfd;
        conn->ip = co::ip_str(&addr);
        conn->port = ntoh16(addr.sin_port);

        // create a new coroutine for this connection
        COLOG << "server accept new connection: " << conn->ip << ":" << conn->port;
        co::go(on_new_connection, conn);
    }
}

void client_fun() {
    sock_t fd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr;
    co::init_ip_addr(&addr, FLG_ip.c_str(), FLG_port);

    if(co::connect(fd, &addr, sizeof(addr), -1)== -1) 
    {
        COLOG <<"fd:" <<fd <<" connect error";
        return ;
    }
    co::set_tcp_nodelay(fd);

    char buf[8] = { 0 };

    while (true) {
        COLOG << "client send ping";
        int r = co::send(fd, "ping", 4);
        if (r == -1) {
            COLOG << "client send error: " << co::strerror();
            break;
        }

        r = co::recv(fd, buf, 8);
        if (r == -1) {
            COLOG << "client recv error: " << co::strerror();
            break;
        } else if (r == 0) {
            COLOG << "server close the connection";
            break;
        } else {
            COLOG << "client recv " << fastring(buf, r) << '\n';
            // co::sleep(3000);
        }
    }

    co::close(fd);
}

int main(int argc, char** argv) {
    flag::init(argc, argv);
    log::init();

    go(server_fun);
    sleep::ms(32);
    for(size_t i=0;i<1000;i++){
        go(client_fun);
    }

    sleep::sec(60);
    co::stop();
    std::cout<<counter<<std::endl;

    return 0;
}
