#include "../inc/ft_ping.h"
void receive_ping(int sockfd) {
    while (1) {
        char                buffer[1024];
        struct sockaddr_in  src_addr;
        socklen_t           addr_len = sizeof(src_addr);
        struct iovec        iov = {.iov_base = buffer, .iov_len = sizeof(buffer)};
        struct msghdr       msg = {.msg_name = &src_addr, .msg_namelen = addr_len, .msg_iov = &iov, .msg_iovlen = 1};

        ssize_t bytes_received = recvmsg(sockfd, &msg, 0);

        if (bytes_received < 0) {
            return;
        }

        struct timeval tv_recv;
        gettimeofday(&tv_recv, NULL);

        // Extract IP header
        struct iphdr *ip = (struct iphdr *)buffer;
        int ip_hdr_len = ip->ihl * 4;

        // Check if packet is large enough to contain ICMP header
        if (bytes_received < (ssize_t)(ip_hdr_len + sizeof(struct icmphdr))) continue;

        // Extract ICMP header
        struct icmphdr *icmp = (struct icmphdr *)(buffer + ip_hdr_len);

        // Verify it's an echo reply for us
        if (icmp->type == ICMP_ECHOREPLY) {
            if (ntohs(icmp->un.echo.id) == g_config.pid) {
                g_stats.packets_received++;
                
                // Extract the timestamp to calculate RTT
                struct timeval tv_send;
                memcpy(&tv_send, buffer + ip_hdr_len + sizeof(struct icmphdr), sizeof(struct timeval));
                double rtt = get_time_diff(&tv_send, &tv_recv);
                
                // Update stats
                g_stats.sum_rtt += rtt;
                if (g_stats.packets_received == 1 || rtt < g_stats.min_rtt) g_stats.min_rtt = rtt;
                if (rtt > g_stats.max_rtt) g_stats.max_rtt = rtt;

                // Format Source IP (we DO NOT do reverse DNS here per the subject instructions)
                char src_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &src_addr.sin_addr, src_ip, sizeof(src_ip));

                printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
                       bytes_received - ip_hdr_len, src_ip, ntohs(icmp->un.echo.sequence), ip->ttl, rtt);
                return;
            }
        } else if (g_config.verbose) {
            // If it's an error packet and verbose is on, print it
            char src_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &src_addr.sin_addr, src_ip, sizeof(src_ip));
            printf("%ld bytes from %s: type = %d, code = %d\n", bytes_received - ip_hdr_len, src_ip, icmp->type, icmp->code);
            return;
        }
    }
}
