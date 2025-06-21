#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "socket.h"
#include "util.h"

void socket_init(socket_t *sock)
{
    sock->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock->fd == -1) fatal("socket() error");
}

void socket_free(socket_t *sock)
{
    if (sock->fd != -1) {
        close(sock->fd);
        sock->fd = -1;
    }
}

void socket_bind(socket_t *sock, inetaddr_t *iadr)
{
    if (bind(sock->fd, (struct sockaddr *) &iadr->addr, iadr->len) == -1) {
        fatal("bind() error");
    }
}

void socket_listen(socket_t *sock)
{
    if (listen(sock->fd, SOMAXCONN) == -1) fatal("listen() error");
}

int socket_accept(socket_t *sock, inetaddr_t *iadr)
{
    int clnt_sockfd = accept(sock->fd, (struct sockaddr *) &iadr->addr,
                             &iadr->len);
    if (clnt_sockfd == -1) fatal("accept() error");
    return clnt_sockfd;
}

void socket_setnoblock(socket_t *sock)
{
    fcntl(sock->fd, F_SETFL, fcntl(sock->fd, F_GETFL) | O_NONBLOCK);
}
