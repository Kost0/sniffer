#include "IPHandler.h"
#include <netinet/ip.h>
#include <arpa/inet.h>

void IPHandler::handle(const uint8_t* data, int len,
                       int offset, PacketInfo& out) {
    if (offset + static_cast<int>(sizeof(iphdr)) > len) return;

    const auto* iph = reinterpret_cast<const iphdr*>(data + offset);
    char buf[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &iph->saddr, buf, sizeof(buf));
    out.srcIp = buf;
    inet_ntop(AF_INET, &iph->daddr, buf, sizeof(buf));
    out.dstIp = buf;

    int ipHdrLen = iph->ihl * 4;
    if (next_) {
        next_->handle(data, len, offset + ipHdrLen, out);
    }
}
