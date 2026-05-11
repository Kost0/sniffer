#include "ProtocolParserFactory.h"
#include "IProtocolParser.h"
#include "PacketInfo.h"
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>


struct TCPParser : IProtocolParser {
    PacketInfo parse(const uint8_t* data, int len) override {
        PacketInfo p;
        p.protocol = "TCP";
        if (len >= static_cast<int>(sizeof(tcphdr))) {
            const auto* h = reinterpret_cast<const tcphdr*>(data);
            p.srcPort = ntohs(h->source);
            p.dstPort = ntohs(h->dest);
        }
        return p;
    }
};

struct UDPParser : IProtocolParser {
    PacketInfo parse(const uint8_t* data, int len) override {
        PacketInfo p;
        p.protocol = "UDP";
        if (len >= static_cast<int>(sizeof(udphdr))) {
            const auto* h = reinterpret_cast<const udphdr*>(data);
            p.srcPort = ntohs(h->source);
            p.dstPort = ntohs(h->dest);
        }
        return p;
    }
};

struct UnsupportedParser : IProtocolParser {
    PacketInfo parse(const uint8_t*, int) override {
        return PacketInfo{};
    }
};


std::unique_ptr<IProtocolParser>
ProtocolParserFactory::create(uint8_t protocol) {
    switch (protocol) {
        case 6:  return std::make_unique<TCPParser>();
        case 17: return std::make_unique<UDPParser>();
        default: return std::make_unique<UnsupportedParser>();
    }
}
