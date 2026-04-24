#pragma once
#include "PacketInfo.h"
#include "AbstractLayerHandler.h"
#include <memory>
#include <cstdint>

class ProtocolParser {
public:
    ProtocolParser();
    PacketInfo parse(const uint8_t* raw, int len);

private:
    std::unique_ptr<AbstractLayerHandler> chain_;
};
