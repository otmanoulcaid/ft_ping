#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <sys/time.h>
# include <signal.h>
# include <stdbool.h>

# define PING_PACKET_SIZE 64 // 56 bytes data + 8 bytes ICMP header
# define PING_DEFAULT_TTL 64

// Struct to hold all configuration options
typedef struct s_ping_config {
    char                *dest_name;
    char                dest_ip[INET_ADDRSTRLEN];
    struct sockaddr_in  dest_addr;
    
    // Options
    bool                verbose;        // -v
    bool                help;           // -?
    
    // Core parameters
    int                 pid;            // Identifier for our ping packets
    int                 sequence;       // Sequence number for ICMP packets
} t_ping_config;

// Struct to hold statistics
typedef struct s_ping_stats {
    long                packets_sent;
    long                packets_received;
    long                packets_errors;
    struct timeval      start_time;
    double              min_rtt;
    double              max_rtt;
    double              sum_rtt;
} t_ping_stats;

// Global state pointer for signal handler access
extern t_ping_config g_config;
extern t_ping_stats g_stats;

// Function prototypes
void    parse_args(int argc, char **argv);
void    resolve_dns(void);
void    print_help(void);

int     create_socket(void);
void    send_ping(int sockfd);
void    receive_ping(int sockfd);

unsigned short calculate_checksum(void *b, int len);
double         get_time_diff(struct timeval *start, struct timeval *end);

#endif
