#pragma once
#include "IPacketObserver.h"
#include <pcap.h>
#include <string>
#include <vector>
#include <memory>

class CaptureEngine {
public:
    CaptureEngine();
    ~CaptureEngine();

    bool open(const std::string& iface);
    bool openOffline(const std::string& file);
    bool setFilter(const std::string& expr);
    void startCapture(int maxPackets = 0);
    void stop();

    void addObserver(std::shared_ptr<IPacketObserver> obs);

private:
    static void pcapCallback(u_char* user,
                             const struct pcap_pkthdr* hdr,
                             const u_char* bytes);
    void notify(const PacketInfo& pkt);

    pcap_t* handle_ = nullptr;
    std::vector<std::shared_ptr<IPacketObserver>> observers_;
};
