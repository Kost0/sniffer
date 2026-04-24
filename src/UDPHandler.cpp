#include "UDPHandler.h"
#include <netinet/udp.h>
#include <arpa/inet.h>

void UDPHandler::handle(const uint8_t* data, int len,
                        int offset, PacketInfo& out) {
    if (offset + static_cast<int>(sizeof(udphdr)) > len) return;

    const auto* udph = reinterpret_cast<const udphdr*>(data + offset);
    out.protocol = "UDP";
    out.srcPort  = ntohs(udph->source);
    out.dstPort  = ntohs(udph->dest);
}
