#ifndef SOCKET_H
#define SOCKET_H

#include "inetaddr.h"

typedef struct {
    int fd;
} socket_t;

void socket_init(socket_t *sock);
void socket_free(socket_t *sock);
void socket_bind(socket_t *sock, inetaddr_t *iadr);
void socket_listen(socket_t *sock);
int socket_accept(socket_t *sock, inetaddr_t *iadr);
void socket_setnoblock(socket_t *sock);

#endif // SOCKET_H
