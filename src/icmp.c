#include "ft_ping.h"

void send_ping(int sockfd) {
    char packet[PING_PACKET_SIZE];
    struct icmphdr *icmp = (struct icmphdr *)packet;

    // Clear the packet
    memset(packet, 0, PING_PACKET_SIZE);

    // Build ICMP header
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->un.echo.id = htons(g_config.pid);
    icmp->un.echo.sequence = htons(++g_config.sequence);

    // Include timestamp in the data payload
    struct timeval tv;
    gettimeofday(&tv, NULL);
    memcpy(packet + sizeof(struct icmphdr), &tv, sizeof(struct timeval));

    // Pad the rest with pattern or arbitrary data
    for (size_t i = sizeof(struct icmphdr) + sizeof(struct timeval); i < PING_PACKET_SIZE; i++) {
        packet[i] = i & 0xFF;
    }

    // Calculate checksum over the whole packet (header + data)
    icmp->checksum = 0;
    icmp->checksum = calculate_checksum(packet, PING_PACKET_SIZE);

    // Send the packet
    if (sendto(sockfd, packet, PING_PACKET_SIZE, 0, 
               (struct sockaddr *)&g_config.dest_addr, sizeof(g_config.dest_addr)) <= 0) {
        perror("ft_ping: sendto");
    } else {
        g_stats.packets_sent++;
    }
}
