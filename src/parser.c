#include "../inc/ft_ping.h"
void print_help(void) {
    printf("Usage: ft_ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
    printf(" Options valid for all request types:\n\n");
    printf("  -v, --verbose              verbose output\n");
    printf("  -?, --help                 give this help list\n");
    printf("\nMandatory options to long options are mandatory for short options too.\n");
    printf("\nReport bugs to <student@42.fr>.\n");
}

void parse_args(int argc, char **argv) {
    int i = 0;
    
    while (++i < argc && argv[i]) {
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
                g_config.verbose = true;
            else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
                g_config.help = true;
            else {
                fprintf(stderr, "ft_ping: invalid option -- '%s'\n", argv[i]);
                fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
                exit(1);
            }
        } else {
            // Assume it's the destination host
            if (!g_config.dest_name)
                g_config.dest_name = argv[i];
        }
    }
}
