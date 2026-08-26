#include "../inc/ft_ping.h"

// Initialize globals
t_ping_config g_config = {0};
t_ping_stats g_stats = {0};

void handle_sigint(int sig) {
    (void)sig;
    // Calculate total time
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    double total_time = (end_time.tv_sec - g_stats.start_time.tv_sec) * 1000.0 + 
                        (end_time.tv_usec - g_stats.start_time.tv_usec) / 1000.0;
    
    // Print statistics
    printf("\n--- %s ping statistics ---\n", g_config.dest_name);
    long loss = 0;
    if (g_stats.packets_sent > 0) {
        loss = ((g_stats.packets_sent - g_stats.packets_received) * 100) / g_stats.packets_sent;
    }
    
    printf("%ld packets transmitted, %ld received, %ld%% packet loss, time %.0fms\n", 
           g_stats.packets_sent, g_stats.packets_received, loss, total_time);
           
    if (g_stats.packets_received > 0) {
        double avg = g_stats.sum_rtt / g_stats.packets_received;
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/0.000 ms\n", 
               g_stats.min_rtt, avg, g_stats.max_rtt);
    }
    
    exit(0);
}

int main(int argc, char **argv) {
    // 1. Parse Arguments
    parse_args(argc, argv);
    
    if (g_config.help) {
        print_help();
        return (0);
    }
    
    if (!g_config.dest_name) {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
        return (1);
    }

    // 2. Resolve DNS
    resolve_dns();
    
    // Setup signal handler for Ctrl+C
    signal(SIGINT, handle_sigint);
    gettimeofday(&g_stats.start_time, NULL);
    g_config.pid = getpid() & 0xFFFF; // 16-bit PID for ICMP identifier
    
    // 3. Create Socket
    int sockfd = create_socket();
    
    printf("PING %s (%s): %d data bytes\n", g_config.dest_name, g_config.dest_ip, PING_PACKET_SIZE - 8);
    
    // Main Loop
    while (1) {
        send_ping(sockfd);
        receive_ping(sockfd);
        sleep(1); // Normal ping waits 1 second between requests
    }
    
    return (0);
}
