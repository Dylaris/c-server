#ifndef INETADDR_H
#define INETADDR_H

#include <arpa/inet.h>

typedef struct {
    struct sockaddr_in addr;
    socklen_t len;
} inetaddr_t;

void inetaddr_init(inetaddr_t *iadr, const char *ip, uint16_t port);
void inetaddr_free(inetaddr_t *iadr);

#endif // INETADDR_H
