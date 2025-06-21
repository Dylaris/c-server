#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "epoll.h"
#include "util.h"

void epoll_init(epoll_t *epoll)
{
    epoll->epfd = epoll_create1(0);
    if (epoll->epfd == -1) fatal("epoll_create1() error");

    epoll->events = malloc(sizeof(struct epoll_event)*MAX_EVENTS);
    if (!epoll->events) fatal("out of memory");
    memset(epoll->events, 0, sizeof(struct epoll_event)*MAX_EVENTS); 
}

void epoll_free(epoll_t *epoll)
{
    if (epoll->epfd == -1) {
        close(epoll->epfd);
        epoll->epfd = -1;
    }
    if (epoll->events) free(epoll->events);
    epoll->events = NULL;
}

void epoll_add(epoll_t *epoll, int fd, uint32_t op)
{
    struct epoll_event ev = {0};
    ev.data.fd = fd;
    ev.events = op;
    if (epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        fatal("epoll_ctl() error");
    }
}

events_t epoll_poll(epoll_t *epoll, int timeout)
{
    events_t active_events = {0};
    int nfds = epoll_wait(epoll->epfd, epoll->events, MAX_EVENTS, timeout);
    if (nfds == -1) fatal("epoll_wait() error");
    for (int i = 0; i < nfds; i++) {
        da_append(&active_events, epoll->events[i]);
    }
    return active_events;
}
