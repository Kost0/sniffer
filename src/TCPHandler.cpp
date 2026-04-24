#include "TCPHandler.h"
#include <netinet/tcp.h>
#include <arpa/inet.h>

void TCPHandler::handle(const uint8_t* data, int len,
                        int offset, PacketInfo& out) {
    if (offset + static_cast<int>(sizeof(tcphdr)) > len) return;

    const auto* tcph = reinterpret_cast<const tcphdr*>(data + offset);
    out.protocol = "TCP";
    out.srcPort  = ntohs(tcph->source);
    out.dstPort  = ntohs(tcph->dest);
}
