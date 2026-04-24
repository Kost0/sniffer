#include "EthernetHandler.h"
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <cstring>

static constexpr int ETH_HDR_LEN = 14;
static constexpr uint16_t ETHERTYPE_IP4 = 0x0800;

void EthernetHandler::handle(const uint8_t* data, int len,
                              int offset, PacketInfo& out) {
    if (offset + ETH_HDR_LEN > len) return;

    uint16_t ethertype = static_cast<uint16_t>(
        (data[offset + 12] << 8) | data[offset + 13]);

    if (ethertype == ETHERTYPE_IP4 && next_) {
        next_->handle(data, len, offset + ETH_HDR_LEN, out);
    }
}
