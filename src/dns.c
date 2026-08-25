#include "ft_ping.h"

void resolve_dns(void) {
    struct addrinfo hints;
    struct addrinfo *result;
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4 only per subject
    // Remove SOCK_RAW and IPPROTO_ICMP constraints as they can cause getaddrinfo to fail
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;

    int s = getaddrinfo(g_config.dest_name, NULL, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "ft_ping: unknown host\n");
        exit(1);
    }

    // Save the resolved address
    memcpy(&g_config.dest_addr, result->ai_addr, sizeof(struct sockaddr_in));
    
    // Convert IP to string for printing
    if (!inet_ntop(AF_INET, &g_config.dest_addr.sin_addr, g_config.dest_ip, INET_ADDRSTRLEN)) {
        perror("inet_ntop");
        freeaddrinfo(result);
        exit(1);
    }

    freeaddrinfo(result);
}
