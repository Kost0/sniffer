#pragma once
#include "IPacketObserver.h"
#include <map>
#include <string>
#include <cstdint>

class StatisticsCollector : public IPacketObserver {
public:
    void onPacket(const PacketInfo& pkt) override;
    void printSummary() const;

    uint64_t totalPackets() const { return totalPackets_; }
    uint64_t totalBytes()   const { return totalBytes_;   }

private:
    struct ProtoStat { uint64_t packets = 0; uint64_t bytes = 0; };

    uint64_t totalPackets_ = 0;
    uint64_t totalBytes_   = 0;
    std::map<std::string, ProtoStat> protocolStats_;
    std::map<std::string, uint64_t>  ipTraffic_;
};
