#pragma once
#include "IPacketObserver.h"
#include "StatisticsCollector.h"

class OutputRenderer : public IPacketObserver {
public:
    void onPacket(const PacketInfo& pkt) override;
    void printSummary(const StatisticsCollector& stats);
};
