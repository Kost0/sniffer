#include "ProtocolParser.h"
#include "EthernetHandler.h"
#include "IPHandler.h"
#include "TCPHandler.h"
#include "UDPHandler.h"
#include <netinet/ip.h>

ProtocolParser::ProtocolParser() {
    auto eth = std::make_unique<EthernetHandler>();
    auto ip  = std::make_unique<IPHandler>();
    eth->setNext(std::move(ip));
    chain_ = std::move(eth);
}

#include "ProtocolParserFactory.h"

PacketInfo ProtocolParser::parse(const uint8_t* raw, int len) {
    PacketInfo info;
    info.length = len;

    chain_->handle(raw, len, 0, info);

    constexpr int ETH_HDR_LEN = 14;
    if (len < ETH_HDR_LEN + static_cast<int>(sizeof(iphdr))) return info;

    const auto* iph = reinterpret_cast<const iphdr*>(raw + ETH_HDR_LEN);
    int ipHdrLen    = iph->ihl * 4;
    int transportOff = ETH_HDR_LEN + ipHdrLen;

    auto parser = ProtocolParserFactory::create(iph->protocol);
    PacketInfo transport = parser->parse(raw + transportOff,
                                         len - transportOff);
    info.protocol = transport.protocol;
    info.srcPort  = transport.srcPort;
    info.dstPort  = transport.dstPort;

    return info;
}
