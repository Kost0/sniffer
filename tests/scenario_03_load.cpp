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

    std::cout << "--- Assertions ---\n";

    assert(stats.totalPackets() == 1000);
    std::cout << "[OK] totalPackets == 1000\n";

    assert(stats.totalBytes() == 600*100 + 400*50);
    std::cout << "[OK] totalBytes == 80000\n";

    assert(stats.packetsForProtocol("TCP") == 600);
    std::cout << "[OK] TCP packets == 600\n";

    assert(stats.packetsForProtocol("UDP") == 400);
    std::cout << "[OK] UDP packets == 400\n";

    assert(stats.bytesForProtocol("TCP") == 60000);
    std::cout << "[OK] TCP bytes == 60000\n";

    assert(stats.bytesForProtocol("UDP") == 20000);
    std::cout << "[OK] UDP bytes == 20000\n";

    assert(stats.bytesForIp("10.0.0.1") == 60000);
    std::cout << "[OK] IP 10.0.0.1 bytes == 60000\n";

    assert(stats.bytesForIp("10.0.0.2") == 60000);
    std::cout << "[OK] IP 10.0.0.2 bytes == 60000\n";

    assert(stats.bytesForIp("10.0.0.3") == 20000);
    assert(stats.bytesForIp("10.0.0.4") == 20000);
    std::cout << "[OK] UDP IP counters correct\n";

    std::cout << "\nSummary\n";
    stats.printSummary();

    std::cout << "\n[PASSED] Scenario 3\n";
    return 0;
}
