#pragma once
#include "PacketInfo.h"

struct IPacketObserver {
    virtual ~IPacketObserver() = default;
    virtual void onPacket(const PacketInfo& pkt) = 0;
};
