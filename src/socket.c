#include "../inc/ft_ping.h"
int create_socket(void) {
    int sockfd;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("ft_ping: socket");
        fprintf(stderr, "Hint: ft_ping must run as root.\n");
        exit(1);
    }

    // Set TTL if we were implementing it as a bonus
    int ttl = PING_DEFAULT_TTL;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) != 0) {
        perror("ft_ping: setsockopt TTL");
        // Don't exit, not fatal
    }

    // Set a receive timeout so recvfrom doesn't block forever
    struct timeval tv;
    tv.tv_sec = 1; // 1 second timeout
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) != 0) {
        perror("ft_ping: setsockopt RCVTIMEO");
    }

    return (sockfd);
}
