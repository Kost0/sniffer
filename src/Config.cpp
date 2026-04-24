#include "Config.h"
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

void Config::parse(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "i:f:n:vh")) != -1) {
        switch (opt) {
            case 'i': interface = optarg;             break;
            case 'f': filter    = optarg;             break;
            case 'n': count     = std::atoi(optarg);  break;
            case 'v': verbose   = true;               break;
            case 'h':
            default:
                std::printf(
                    "Usage: sniffer [OPTIONS]\n"
                    "  -i <iface>   Network interface to capture on\n"
                    "  -f <filter>  BPF filter expression (e.g. \"tcp port 80\")\n"
                    "  -n <count>   Stop after <count> packets\n"
                    "  -v           Verbose mode\n"
                    "  -h           Show this help\n"
                );
                std::exit(0);
        }
    }
}
