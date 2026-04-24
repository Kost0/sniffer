#include "StatisticsCollector.h"
#include <cstdio>
#include <vector>
#include <algorithm>

void StatisticsCollector::onPacket(const PacketInfo& pkt) {
    ++totalPackets_;
    totalBytes_ += static_cast<uint64_t>(pkt.length);
    protocolStats_[pkt.protocol].packets++;
    protocolStats_[pkt.protocol].bytes += static_cast<uint64_t>(pkt.length);
    if (!pkt.srcIp.empty()) ipTraffic_[pkt.srcIp] += pkt.length;
    if (!pkt.dstIp.empty()) ipTraffic_[pkt.dstIp] += pkt.length;
}

void StatisticsCollector::printSummary() const {
    std::printf("\nCapture Summary\n");
    std::printf("Total packets : %llu\n",
                static_cast<unsigned long long>(totalPackets_));
    std::printf("Total bytes   : %llu\n",
                static_cast<unsigned long long>(totalBytes_));

    std::printf("\nProtocol breakdown:\n");
    for (const auto& [proto, s] : protocolStats_) {
        std::printf("  %-8s  %6llu pkts  %9llu bytes\n",
                    proto.c_str(),
                    static_cast<unsigned long long>(s.packets),
                    static_cast<unsigned long long>(s.bytes));
    }

    std::printf("\nTop-5 IP addresses by traffic:\n");
    std::vector<std::pair<std::string, uint64_t>> sorted(
        ipTraffic_.begin(), ipTraffic_.end());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });

    int cnt = 0;
    for (const auto& [ip, bytes] : sorted) {
        std::printf("  %-20s  %9llu bytes\n",
                    ip.c_str(),
                    static_cast<unsigned long long>(bytes));
        if (++cnt >= 5) break;
    }
    std::printf("\n");
}
