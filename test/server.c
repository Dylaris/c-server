#include <stdio.h>

#include <unistd.h>
#include <errno.h>

#include "inetaddr.h"
#include "socket.h"
#include "epoll.h"
#include "util.h"

#define HOST "127.0.0.1" 
#define PORT 9190

static void handle_read_event(int sockfd)
{
    while (1) {
        char buf[1024] = {0};
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from client fd %d: %s", sockfd, buf);
            write(sockfd, buf, read_bytes);
        } else if (read_bytes == 0) {
            printf("EOF client fd %d disconnected\n", sockfd);
            close(sockfd);
            break;
        } else if (read_bytes == -1 && errno == EINTR) {
            printf("continue reading...\n");
            continue;
        } else if (read_bytes == -1 &&
                (errno == EAGAIN || errno == EWOULDBLOCK)) {
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
    }
}

int main(void)
{
    socket_t serv_sock;
    socket_init(&serv_sock);

    inetaddr_t serv_addr;
    inetaddr_init(&serv_addr, HOST, PORT);

    socket_bind(&serv_sock, &serv_addr);
    socket_listen(&serv_sock);

    socket_setnoblock(&serv_sock);

    epoll_t ep;
    epoll_init(&ep);
    epoll_add(&ep, serv_sock.fd, EPOLLIN | EPOLLET);

    while (1) {
        events_t events = epoll_poll(&ep, -1);
        int nfds = events.count;
        for (int i = 0; i < nfds; i++) {
            int cur_evfd = events.items[i].data.fd;
            if (cur_evfd == serv_sock.fd) {
                /* New client */
                inetaddr_t clnt_addr = {0};
                socket_t clnt_sock = {0};
                clnt_sock.fd = socket_accept(&serv_sock, &clnt_addr);
                printf("New client fd %d! IP: %s Port: %d\n",
                        clnt_sock.fd,
                        inet_ntoa(clnt_addr.addr.sin_addr),
                        ntohs(clnt_addr.addr.sin_port));
                socket_setnoblock(&clnt_sock);
                epoll_add(&ep, clnt_sock.fd, EPOLLIN | EPOLLET);
            } else if (cur_evfd & EPOLLIN) {
                handle_read_event(cur_evfd);
            } else {
                printf("something else happened\n");
            }
        }
    }

    inetaddr_free(&serv_addr);
    socket_free(&serv_sock);
    epoll_free(&ep);

    return 0;
}
