#include <iostream>
#include <cassert>
#include <vector>
#include "ProtocolParser.h"
#include "StatisticsCollector.h"
#include "PacketInfo.h"

int main() {
    std::cout << "Scenario 2: Malformed Packet Resilience ===\n\n";

    ProtocolParser parser;
    StatisticsCollector stats;
    int crashCount = 0;

    struct TestCase { std::string name; std::vector<uint8_t> data; };
    std::vector<TestCase> cases = {
        { "Empty buffer",                {}                             },
        { "1 byte",                      {0x00}                        },
        { "Only Ethernet header (14B)",  std::vector<uint8_t>(14, 0)   },
        { "Ethernet + partial IP (5B)",  std::vector<uint8_t>(19, 0)   },
        { "Non-IPv4 EtherType (ARP)",    [](){
            std::vector<uint8_t> b(60, 0);
            b[12]=0x08; b[13]=0x06; return b; }()                     },
        { "Ethernet+IP but no transport",std::vector<uint8_t>(34, 0)   },
        { "All zeros (60 bytes)",        std::vector<uint8_t>(60, 0)   },
        { "All 0xFF (60 bytes)",         std::vector<uint8_t>(60,0xFF) },
    };

    for (auto& tc : cases) {
        std::cout << "Testing: " << tc.name << "\n";
        try {
            PacketInfo pkt = parser.parse(tc.data.data(),
                                          static_cast<int>(tc.data.size()));
            if (!pkt.protocol.empty()) {
                stats.onPacket(pkt);
            }
        } catch (...) {
            std::cout << "EXCEPTION (unexpected!)\n";
            ++crashCount;
        }
    }

    assert(crashCount == 0);
    assert(stats.totalPackets() >= 0);

    std::cout << "\n[PASSED] Scenario 2\n";
    return 0;
}
