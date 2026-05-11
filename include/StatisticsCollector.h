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

    uint64_t packetsForProtocol(const std::string& proto) const {
        auto it = protocolStats_.find(proto);
        return it != protocolStats_.end() ? it->second.packets : 0;
    }
    uint64_t bytesForProtocol(const std::string& proto) const {
        auto it = protocolStats_.find(proto);
        return it != protocolStats_.end() ? it->second.bytes : 0;
    }
    uint64_t bytesForIp(const std::string& ip) const {
        auto it = ipTraffic_.find(ip);
        return it != ipTraffic_.end() ? it->second : 0;
    }
    bool hasProtocol(const std::string& proto) const {
        return protocolStats_.count(proto) > 0;
    }

    void reset() {
        totalPackets_ = 0;
        totalBytes_   = 0;
        protocolStats_.clear();
        ipTraffic_.clear();
    }

private:
    struct ProtoStat { uint64_t packets = 0; uint64_t bytes = 0; };

    uint64_t totalPackets_ = 0;
    uint64_t totalBytes_   = 0;
    std::map<std::string, ProtoStat> protocolStats_;
    std::map<std::string, uint64_t>  ipTraffic_;
};
