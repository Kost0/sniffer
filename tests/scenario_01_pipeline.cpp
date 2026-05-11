#include <iostream>
#include <cassert>
#include <vector>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include "EthernetHandler.h"
#include "IPHandler.h"
#include "TCPHandler.h"
#include "UDPHandler.h"
#include "ProtocolParserFactory.h"
#include "ProtocolParser.h"
#include "StatisticsCollector.h"
#include "OutputRenderer.h"
#include "PacketInfo.h"

static std::vector<uint8_t> makeTCPFrame(const char* src, const char* dst,
                                          uint16_t sport, uint16_t dport,
                                          int totalLen = 100) {
    std::vector<uint8_t> buf(14 + sizeof(iphdr) + sizeof(tcphdr), 0);
    buf[12] = 0x08; buf[13] = 0x00;
    auto* iph = reinterpret_cast<iphdr*>(buf.data() + 14);
    iph->ihl = 5; iph->version = 4; iph->protocol = 6;
    inet_pton(AF_INET, src, &iph->saddr);
    inet_pton(AF_INET, dst, &iph->daddr);
    auto* tcph = reinterpret_cast<tcphdr*>(buf.data() + 14 + sizeof(iphdr));
    tcph->source = htons(sport);
    tcph->dest   = htons(dport);
    return buf;
}

static std::vector<uint8_t> makeUDPFrame(const char* src, const char* dst,
                                          uint16_t sport, uint16_t dport) {
    std::vector<uint8_t> buf(14 + sizeof(iphdr) + sizeof(udphdr), 0);
    buf[12] = 0x08; buf[13] = 0x00;
    auto* iph = reinterpret_cast<iphdr*>(buf.data() + 14);
    iph->ihl = 5; iph->version = 4; iph->protocol = 17;
    inet_pton(AF_INET, src, &iph->saddr);
    inet_pton(AF_INET, dst, &iph->daddr);
    auto* udph = reinterpret_cast<udphdr*>(buf.data() + 14 + sizeof(iphdr));
    udph->source = htons(sport);
    udph->dest   = htons(dport);
    return buf;
}

int main() {
    std::cout << "Scenario 1: Full Packet Processing Pipeline\n";

    ProtocolParser    parser;
    StatisticsCollector stats;
    OutputRenderer    renderer;

    struct RawPkt { std::vector<uint8_t> data; int len; };
    std::vector<RawPkt> packets = {
        { makeTCPFrame("192.168.1.10", "93.184.216.34", 54312, 80),  74  },
        { makeTCPFrame("192.168.1.10", "93.184.216.34", 54313, 443), 120 },
        { makeTCPFrame("93.184.216.34","192.168.1.10",  80, 54312),  52  },
        { makeUDPFrame("192.168.1.10", "8.8.8.8",       12345, 53),  64  },
        { makeUDPFrame("8.8.8.8",      "192.168.1.10",  53, 12345),  80  },
    };

    std::cout << "Packets received\n";
    for (auto& raw : packets) {
        PacketInfo pkt = parser.parse(raw.data.data(),
                                       static_cast<int>(raw.data.size()));
        pkt.length = raw.len;
        if (pkt.protocol.empty()) continue;
        stats.onPacket(pkt);
        renderer.onPacket(pkt);
    }

    std::cout << "\nAssertions\n";

    assert(stats.totalPackets() == 5);
    std::cout << "[OK] totalPackets == 5\n";

    assert(stats.packetsForProtocol("TCP") == 3);
    std::cout << "[OK] TCP packets == 3\n";

    assert(stats.packetsForProtocol("UDP") == 2);
    std::cout << "[OK] UDP packets == 2\n";

    assert(stats.totalBytes() == 74 + 120 + 52 + 64 + 80);
    std::cout << "[OK] totalBytes == 390\n";

    assert(stats.bytesForIp("8.8.8.8") == 64 + 80);
    std::cout << "[OK] 8.8.8.8 bytes == 144\n";

    std::cout << "\nSummary\n";
    renderer.printSummary(stats);

    std::cout << "\n[PASSED] Scenario 1\n";
    return 0;
}
