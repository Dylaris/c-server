#ifndef UTIL_H
#define UTIL_H

#define fatal(fmt, ...)                         \
    do {                                        \
        fprintf(stderr, fmt" at <%s:%d>\n",     \
            ##__VA_ARGS__, __FILE__, __LINE__); \
        exit(1);                                \
    } while (0)

#define da_append(da, item)                                             \
    do {                                                                \
        if ((da)->capacity <= (da)->count) {                            \
            (da)->capacity = (da)->capacity==0 ? 10 : 2*(da)->capacity; \
            (da)->items = realloc((da)->items,                          \
                    sizeof(*(da)->items)*(da)->capacity);               \
            if(!(da)->items) fatal("out of memory");                    \
        }                                                               \
        (da)->items[(da)->count++] = (item);                            \
    } while (0)

#define da_free(da)                         \
    do {                                    \
        if ((da)->items) free((da)->items); \
        (da)->items = NULL;                 \
        (da)->count = 0;                    \
        (da)->capcity = 0;                  \
    } while (0)

#endif // UTIL_H
