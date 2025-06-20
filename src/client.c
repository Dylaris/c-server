#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define HOST "127.0.0.1" 
#define PORT 9190

#define fatal(fmt, ...)                                 \
    do {                                                \
        fprintf(stderr, "Client: "fmt" at <%s:%d>\n",   \
            ##__VA_ARGS__, __FILE__, __LINE__);         \
        exit(1);                                        \
    } while (0)

int main(void)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) fatal("socket() error");

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(HOST);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *) &serv_addr,
               sizeof(serv_addr)) == -1) {
        fatal("connect() error");
    }

    while (1) {
        printf("input: ");
        fflush(stdout);

        char buf[1024] = {0};
        fgets(buf, sizeof(buf)-1, stdin);

        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        if (write_bytes == -1) {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        memset(buf, 0, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if (read_bytes > 0) {
            printf("message from server: %s", buf);
        } else if (read_bytes == 0) {
            printf("server socket disconnected\n");
            break;
        } else {
            close(sockfd);
            fatal("read() error");
        }
    }

    close(sockfd);
    return 0;
}
