#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define fatal(fmt, ...)                                 \
    do {                                                \
        fprintf(stderr, "Server: "fmt" at <%s:%d>\n",   \
            ##__VA_ARGS__, __FILE__, __LINE__);         \
        exit(1);                                        \
    } while (0)

#define HOST "127.0.0.1" 
#define PORT 9190
#define MAX_EVENTS 1024

#define setnonblocking(fd) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK)

int main(void)
{
    int serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sockfd == -1) fatal("socket() error");

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(HOST);
    serv_addr.sin_port = htons(PORT);

    if (bind(serv_sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) == -1) {
        fatal("bind() error");
    }

    if (listen(serv_sockfd, SOMAXCONN) == -1) fatal("listen() error");


    int epfd = epoll_create1(0);
    if (epfd == -1) fatal("epoll_create1() error");

    struct epoll_event events[MAX_EVENTS] = {0}, ev = {0};
    ev.data.fd = serv_sockfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(serv_sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sockfd, &ev);

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) fatal("epoll_wait() error");

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == serv_sockfd) {
                /* New client */
                struct sockaddr_in clnt_addr = {0};
                socklen_t clnt_addr_size = sizeof(clnt_addr);
                int clnt_sockfd = accept(serv_sockfd, (struct sockaddr *) &clnt_addr,
                                       &clnt_addr_size);
                if (clnt_sockfd == -1) fatal("accept() error");

                memset(&ev, 0, sizeof(ev));
                ev.data.fd = clnt_sockfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev);
            } else if (events[i].data.fd & EPOLLIN) {
                /* Read event in clnt_sockfd */
                int clnt_sockfd = events[i].data.fd;
                while (1) {
                    char buf[1024] = {0};
                    ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf));
                    if (read_bytes > 0) {
                        printf("message from client fd %d: %s", clnt_sockfd, buf);
                        write(clnt_sockfd, buf, read_bytes);
                    } else if (read_bytes == 0) {
                        printf("client fd %d disconnected\n", clnt_sockfd);
                        close(clnt_sockfd);
                        break;
                    } else if (read_bytes == -1 && errno == EINTR) {
                        /* Normal interrupt */
                        printf("continue reading...\n");
                        continue;
                    } else if (read_bytes == -1 &&
                            (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    }
                }
            } else {
                fatal("other things happend");
            }
        }
    }

    close(serv_sockfd);
    return 0;
}
