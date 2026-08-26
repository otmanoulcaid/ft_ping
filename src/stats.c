#include "../inc/ft_ping.h"
// Statistics are currently handled directly in:
// - main.c (printing on SIGINT)
// - receiver.c (updating on receive)
// - icmp.c (updating on send)
//
// If the bonus options (like -f, -l, standard deviation) require more complex 
// tracking, this file will be useful to encapsulate the logic.
