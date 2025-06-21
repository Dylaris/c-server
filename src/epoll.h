#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

#define MAX_EVENTS 1024

typedef struct {
    struct epoll_event *items;
    int count;  
    int capacity;
} events_t;

typedef struct {
    int epfd;
    struct epoll_event *events;
} epoll_t;

void epoll_init(epoll_t *epoll);
void epoll_free(epoll_t *epoll);
void epoll_add(epoll_t *epoll, int fd, uint32_t op);
events_t epoll_poll(epoll_t *epoll, int timeout);

#endif // EPOLL_H
