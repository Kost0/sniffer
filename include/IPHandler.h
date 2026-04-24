#pragma once
#include "AbstractLayerHandler.h"

class IPHandler : public AbstractLayerHandler {
public:
    void handle(const uint8_t* data, int len,
                int offset, PacketInfo& out) override;
};
