#include "CaptureEngine.h"
#include "ProtocolParser.h"
#include <cstdio>
#include <cstring>

static ProtocolParser g_parser;

CaptureEngine::CaptureEngine() = default;

CaptureEngine::~CaptureEngine() {
    if (handle_) {
        pcap_close(handle_);
        handle_ = nullptr;
    }
}

bool CaptureEngine::open(const std::string& iface) {
    char errbuf[PCAP_ERRBUF_SIZE];
    handle_ = pcap_open_live(iface.c_str(),
                              65535,
                              1,
                              1000,
                              errbuf);
    if (!handle_) {
        std::fprintf(stderr, "[CaptureEngine] pcap_open_live: %s\n", errbuf);
        return false;
    }
    std::printf("[*] Capturing on interface: %s\n", iface.c_str());
    return true;
}

bool CaptureEngine::openOffline(const std::string& file) {
    char errbuf[PCAP_ERRBUF_SIZE];
    handle_ = pcap_open_offline(file.c_str(), errbuf);
    if (!handle_) {
        std::fprintf(stderr, "[CaptureEngine] pcap_open_offline: %s\n", errbuf);
        return false;
    }
    std::printf("[*] Reading from file: %s\n", file.c_str());
    return true;
}

bool CaptureEngine::setFilter(const std::string& expr) {
    if (!handle_) return false;
    struct bpf_program fp;
    if (pcap_compile(handle_, &fp, expr.c_str(), 1, PCAP_NETMASK_UNKNOWN) == -1) {
        std::fprintf(stderr, "[CaptureEngine] pcap_compile: %s\n",
                     pcap_geterr(handle_));
        return false;
    }
    if (pcap_setfilter(handle_, &fp) == -1) {
        std::fprintf(stderr, "[CaptureEngine] pcap_setfilter: %s\n",
                     pcap_geterr(handle_));
        pcap_freecode(&fp);
        return false;
    }
    pcap_freecode(&fp);
    std::printf("[*] BPF filter applied: %s\n", expr.c_str());
    return true;
}

void CaptureEngine::startCapture(int maxPackets) {
    if (!handle_) return;
    pcap_loop(handle_, maxPackets, &CaptureEngine::pcapCallback,
              reinterpret_cast<u_char*>(this));
}

void CaptureEngine::stop() {
    if (handle_) pcap_breakloop(handle_);
}

void CaptureEngine::addObserver(std::shared_ptr<IPacketObserver> obs) {
    observers_.push_back(std::move(obs));
}

void CaptureEngine::pcapCallback(u_char* user,
                                  const struct pcap_pkthdr* hdr,
                                  const u_char* bytes) {
    auto* self = reinterpret_cast<CaptureEngine*>(user);
    PacketInfo pkt = g_parser.parse(bytes, static_cast<int>(hdr->caplen));
    if (pkt.protocol.empty()) return;
    pkt.timestamp  = hdr->ts;
    pkt.length     = static_cast<int>(hdr->len);
    self->notify(pkt);
}

void CaptureEngine::notify(const PacketInfo& pkt) {
    for (auto& obs : observers_) obs->onPacket(pkt);
}
