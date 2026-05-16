#include <iostream>
#include <cassert>
#include "StatisticsCollector.h"
#include "PacketInfo.h"

static PacketInfo makePkt(const std::string& proto,
                           const std::string& src,
                           const std::string& dst,
                           int len) {
    PacketInfo p;
    p.protocol = proto;
    p.srcIp    = src;
    p.dstIp    = dst;
    p.length   = len;
    return p;
}

int main() {
    std::cout << "Scenario 3: Statistics Accuracy Under Load\n";

    StatisticsCollector stats;

    for (int i = 0; i < 600; ++i)
        stats.onPacket(makePkt("TCP", "10.0.0.1", "10.0.0.2", 100));

    for (int i = 0; i < 400; ++i)
        stats.onPacket(makePkt("UDP", "10.0.0.3", "10.0.0.4", 50));

    std::cout << "Assertions\n";

    assert(stats.totalPackets() == 1000);
    assert(stats.totalBytes() == 600*100 + 400*50);
    assert(stats.packetsForProtocol("TCP") == 600);
    assert(stats.packetsForProtocol("UDP") == 400);
    assert(stats.bytesForProtocol("TCP") == 60000);
    assert(stats.bytesForProtocol("UDP") == 20000);
    assert(stats.bytesForIp("10.0.0.1") == 60000);
    assert(stats.bytesForIp("10.0.0.2") == 60000);
    assert(stats.bytesForIp("10.0.0.3") == 20000);
    assert(stats.bytesForIp("10.0.0.4") == 20000);

    std::cout << "\nSummary\n";
    stats.printSummary();

    std::cout << "\n[PASSED] Scenario 3\n";
    return 0;
}
