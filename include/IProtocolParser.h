#pragma once
#include "PacketInfo.h"
#include <cstdint>

struct IProtocolParser {
    virtual ~IProtocolParser() = default;
    virtual PacketInfo parse(const uint8_t* data, int len) = 0;
};
