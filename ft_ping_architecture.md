# My Modular Architecture for `ft_ping`

To build a clean, robust, and maintainable C project (especially to support all the bonus features later), I've designed a modular architecture. By separating the logic into different modules, I can isolate problems easily and keep the files small.

## 1. Data Structures (`ft_ping.h`)

I use a main context struct that holds the entire state of the program. This avoids using global variables everywhere (except for the one needed for the signal handler).

```c
#ifndef FT_PING_H
# define FT_PING_H

# include <netinet/in.h>
# include <sys/time.h>
# include <stdbool.h>

typedef struct s_ping_config {
    char                *dest_name;       // Original destination string
    char                dest_ip[INET_ADDRSTRLEN]; // Resolved IP address
    struct sockaddr_in  dest_addr;

    // Options
    bool                verbose;           // -v
    bool                help;              // -?
    int                 flood;             // -f (bonus)
    int                 preload;           // -l (bonus)
    int                 numeric;           // -n (bonus)
    int                 timeout_stop;      // -w (bonus)
    int                 timeout_wait;      // -W (bonus)
    char                *pattern;          // -p (bonus)
    int                 dont_route;        // -r (bonus)
    int                 packet_size;       // -s (bonus, default 56)
    int                 ttl;               // --ttl (bonus)
    
    // Core parameters
    int                 pid;
    int                 sequence;
} t_ping_config;

typedef struct s_ping_stats {
    long                packets_sent;
    long                packets_received;
    long                packets_errors;
    struct timeval      start_time;
    double              min_rtt;
    double              max_rtt;
    double              sum_rtt;
    double              sum_sq_rtt; // For standard deviation (bonus)
} t_ping_stats;

#endif
```

## 2. Module Breakdown

### `main.c` (Entry Point & Signal Handling)
- Parses arguments using my parser module.
- Sets up signal handlers (`SIGINT`, `SIGALRM`).
- Initializes the socket using the socket module.
- Contains the main application loop: send packet, wait for response/timeout, update stats, sleep (if not flood ping).

### `parser.c` / `parser.h`
- Handles command line arguments.
- Because `inetutils` uses long options (`--ttl`), I am implementing a custom parser to populate my `t_ping_config` struct.

### `dns.c` / `dns.h`
- Responsible for resolving the target string to an IPv4 address.
- Uses `getaddrinfo` (without specifying `SOCK_RAW` or `IPPROTO_ICMP` in the hints to avoid resolution failures).

### `socket.c` / `socket.h`
- Handles creating the raw socket: `socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)`.
- Applies socket options based on `t_ping_config` (like `IP_TTL`, `SO_RCVTIMEO` for timeouts, `SO_DONTROUTE` for `-r`).

### `icmp.c` / `icmp.h`
- **Packet Building**: Fills the `icmphdr` struct.
- **Checksum Calculation**: Standard Internet Checksum (RFC 1071) function.
- **Payload Management**: Inserts the timestamp and padding into the packet's data section. If the `-p` pattern bonus is active, the padding is filled using the user's hex pattern.

### `receiver.c` / `receiver.h`
- Waits for incoming packets using `recvmsg`.
- **Parsing**: Extracts the IP header and the ICMP header.
- **Validation**: Checks if the received ICMP packet matches my `ft_ping` instance's PID.
- **Error Handling**: Handles ICMP error types and prints verbose output if `-v` is set.

### `stats.c` / `stats.h`
- Manages the `t_ping_stats` struct.
- Calculates and updates the RTT.
- Formats and prints the final output when `Ctrl+C` is pressed.

### `utils.c` / `utils.h`
- Math utilities like calculating time differences (`struct timeval`).
- Future additions: Square root calculation for standard deviation.
