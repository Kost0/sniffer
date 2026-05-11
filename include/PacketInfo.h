#pragma once
#include <string>
#include <cstdint>
#include <sys/time.h>

struct PacketInfo {
    std::string srcIp;
    std::string dstIp;
    uint16_t    srcPort   = 0;
    uint16_t    dstPort   = 0;
    std::string protocol;
    int         length    = 0;
    timeval     timestamp = {};
};
