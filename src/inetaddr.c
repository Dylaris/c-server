#include <string.h>

#include "inetaddr.h"

void inetaddr_init(inetaddr_t *iadr, const char *ip, uint16_t port)
{
    memset(iadr, 0, sizeof(inetaddr_t));
    iadr->addr.sin_family = AF_INET;
    iadr->addr.sin_addr.s_addr = inet_addr(ip);
    iadr->addr.sin_port = htons(port);
    iadr->len = sizeof(iadr->addr);
}

void inetaddr_free(inetaddr_t *iadr)
{
    (void) iadr;
}
