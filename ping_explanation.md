# ft_ping: Understanding the Ping Command

## 1. How `ping` Works Under the Hood

The `ping` command is a fundamental network diagnostic tool I am recoding for this project. It is used to test the reachability of a host on an IP network and to measure the round-trip time (RTT).

### Underlying Protocol: ICMP
Unlike most applications that use TCP or UDP, `ping` operates over **ICMP** (Internet Control Message Protocol), which is an essential part of the IP protocol suite used for network error reporting and diagnostics.

Here is the general flow of how I am implementing the ping operation:
1. **DNS Resolution**: If a hostname (e.g., `google.com`) is provided, I first resolve it to an IP address using DNS (`getaddrinfo`).
2. **Socket Creation**: I open a **Raw Socket** (`AF_INET`, `SOCK_RAW`, `IPPROTO_ICMP`). This requires root (sudo) privileges because raw sockets allow me to bypass the operating system's networking stack and craft raw IP/ICMP headers manually.
3. **Packet Construction**: I build an **ICMP Echo Request** packet (Type 8, Code 0). The packet includes:
   - Type and Code fields.
   - An Identifier and Sequence Number to match requests with replies.
   - A Checksum for data integrity.
   - A data payload (a timestamp to calculate the RTT later, plus padding).
4. **Sending**: I send the packet to the destination using `sendto`.
5. **Receiving**: I listen on the socket for an **ICMP Echo Reply** (Type 0, Code 0) using `recvmsg`. 
6. **RTT Calculation**: When a reply is received, I extract the timestamp from the payload and calculate the RTT.
7. **Signal Handling**: The program runs in a loop (usually once per second) until interrupted (`Ctrl+C` -> `SIGINT`). Upon interruption, it prints statistics (packets sent, received, loss percentage, min/avg/max RTT).

---

## 2. Command Line Arguments and Options

For this project, my reference implementation is `ping` from `inetutils-2.0`.

### Mandatory Options
- **`<destination>`**: An IPv4 address or a hostname. I must handle FQDN (Fully Qualified Domain Name). Per the subject, I must do this "without doing the DNS resolution in the packet return", meaning I just display the IP address in the packet output and do not perform reverse DNS lookups on every received packet.
- **`-v` (Verbose)**: 
  - Normally, `ping` only outputs a line when it receives a successful Echo Reply (Type 0).
  - With `-v`, I also need to print details when I receive *other* types of ICMP packets (errors), such as "Destination Unreachable" or "Time Exceeded" (TTL expired).
- **`-?` (Help)**: Displays the usage manual of my `ft_ping` command. (Note: In bash, `-?` can expand to a wildcard file, so testing might require quoting like `'-?'`).

### Bonus Options (from inetutils-2.0)
- **`-f` (Flood ping)**: Outputs packets as fast as they come back or one hundred times per second, whichever is more. For every request sent, a period `.` is printed, and for every reply, a backspace is printed. *Requires root privileges*.
- **`-l preload`**: Sends the specified number of packets as fast as possible before falling into normal mode of behavior (e.g., 1 packet per second).
- **`-n` (Numeric output)**: Numeric output only. (No reverse DNS resolution when printing output).
- **`-w stop`**: Specifies a timeout, in seconds, before exiting regardless of how many packets have been sent or received.
- **`-W waittime`**: The time to wait for a *single* response, in seconds. 
- **`-p pattern`**: Specifies up to 16 "pad" bytes to fill out the sent packet payload (useful for diagnosing data-dependent network issues, like `-p ff`). 
- **`-r` (Bypass routing)**: Bypass normal routing tables and send directly to a host on an attached network using the `SO_DONTROUTE` socket option.
- **`-s packetsize`**: Specifies the number of data bytes to be sent (default is 56, translating to 64 ICMP data bytes when combined with the header).
- **`-T tos`**: Set the IP Type of Service (TOS) via `IP_TOS`.
- **`--ttl ttl`**: Set the IP Time to Live (TTL) value via `IP_TTL`.
- **`--ip-timestamp`**: Enables the IP timestamp option in the IP header.
