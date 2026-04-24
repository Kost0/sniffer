#include "OutputRenderer.h"
#include <cstdio>

void OutputRenderer::onPacket(const PacketInfo& pkt) {
    std::printf("[%-3s] %-15s:%-5u  ->  %-15s:%-5u  %d bytes\n",
        pkt.protocol.c_str(),
        pkt.srcIp.c_str(),   pkt.srcPort,
        pkt.dstIp.c_str(),   pkt.dstPort,
        pkt.length);
}

void OutputRenderer::printSummary(const StatisticsCollector& stats) {
    stats.printSummary();
}
