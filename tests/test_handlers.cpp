#include <gtest/gtest.h>
#include "EthernetHandler.h"
#include "IPHandler.h"
#include "TCPHandler.h"
#include "UDPHandler.h"
#include "PacketInfo.h"
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <cstring>
#include <vector>


static std::vector<uint8_t> buildTCPFrame(const char* srcIp,
                                           const char* dstIp,
                                           uint16_t srcPort,
                                           uint16_t dstPort) {
    std::vector<uint8_t> buf(14 + sizeof(iphdr) + sizeof(tcphdr), 0);

    buf[12] = 0x08; buf[13] = 0x00;

    auto* iph = reinterpret_cast<iphdr*>(buf.data() + 14);
    iph->ihl      = 5;
    iph->version  = 4;
    iph->protocol = 6;
    inet_pton(AF_INET, srcIp, &iph->saddr);
    inet_pton(AF_INET, dstIp, &iph->daddr);

    auto* tcph = reinterpret_cast<tcphdr*>(buf.data() + 14 + sizeof(iphdr));
    tcph->source = htons(srcPort);
    tcph->dest   = htons(dstPort);

    return buf;
}

static std::vector<uint8_t> buildUDPFrame(const char* srcIp,
                                           const char* dstIp,
                                           uint16_t srcPort,
                                           uint16_t dstPort) {
    std::vector<uint8_t> buf(14 + sizeof(iphdr) + sizeof(udphdr), 0);
    buf[12] = 0x08; buf[13] = 0x00;

    auto* iph = reinterpret_cast<iphdr*>(buf.data() + 14);
    iph->ihl      = 5;
    iph->version  = 4;
    iph->protocol = 17;
    inet_pton(AF_INET, srcIp, &iph->saddr);
    inet_pton(AF_INET, dstIp, &iph->daddr);

    auto* udph = reinterpret_cast<udphdr*>(buf.data() + 14 + sizeof(iphdr));
    udph->source = htons(srcPort);
    udph->dest   = htons(dstPort);

    return buf;
}


TEST(EthernetHandler, IgnoresTruncatedFrame) {
    EthernetHandler eth;
    PacketInfo out;
    uint8_t tiny[5] = {};
    eth.handle(tiny, sizeof(tiny), 0, out);
    EXPECT_EQ(out.srcIp, "");
}

TEST(EthernetHandler, IgnoresNonIPv4EtherType) {
    EthernetHandler eth;
    PacketInfo out;
    uint8_t frame[14 + sizeof(iphdr)] = {};
    frame[12] = 0x08; frame[13] = 0x06;
    eth.handle(frame, sizeof(frame), 0, out);
    EXPECT_EQ(out.srcIp, "");
}

TEST(EthernetHandler, PassesIPv4FrameToNextHandler) {
    auto frame = buildTCPFrame("1.2.3.4", "5.6.7.8", 1000, 80);

    auto tcp = std::make_unique<TCPHandler>();
    auto ip  = std::make_unique<IPHandler>();
    auto eth = std::make_unique<EthernetHandler>();
    ip->setNext(std::move(tcp));
    eth->setNext(std::move(ip));

    PacketInfo out;
    eth->handle(frame.data(), static_cast<int>(frame.size()), 0, out);
    EXPECT_EQ(out.srcIp, "1.2.3.4");
    EXPECT_EQ(out.dstIp, "5.6.7.8");
}

TEST(EthernetHandler, HandlesOffsetCorrectly) {
    uint8_t padding[10] = {};
    auto inner = buildTCPFrame("9.9.9.9", "8.8.8.8", 2000, 443);
    std::vector<uint8_t> buf(padding, padding + 10);
    buf.insert(buf.end(), inner.begin(), inner.end());

    auto tcp = std::make_unique<TCPHandler>();
    auto ip  = std::make_unique<IPHandler>();
    auto eth = std::make_unique<EthernetHandler>();
    ip->setNext(std::move(tcp));
    eth->setNext(std::move(ip));

    PacketInfo out;
    eth->handle(buf.data(), static_cast<int>(buf.size()), 10, out);
    EXPECT_EQ(out.srcIp, "9.9.9.9");
}


TEST(IPHandler, ExtractsSrcIp) {
    auto frame = buildTCPFrame("192.168.1.1", "10.0.0.1", 5000, 80);
    IPHandler ip;
    PacketInfo out;
    ip.handle(frame.data(), static_cast<int>(frame.size()), 14, out);
    EXPECT_EQ(out.srcIp, "192.168.1.1");
}

TEST(IPHandler, ExtractsDstIp) {
    auto frame = buildTCPFrame("192.168.1.1", "10.0.0.1", 5000, 80);
    IPHandler ip;
    PacketInfo out;
    ip.handle(frame.data(), static_cast<int>(frame.size()), 14, out);
    EXPECT_EQ(out.dstIp, "10.0.0.1");
}

TEST(IPHandler, IgnoresTruncatedIPHeader) {
    uint8_t tiny[5] = {};
    IPHandler ip;
    PacketInfo out;
    ip.handle(tiny, sizeof(tiny), 0, out);
    EXPECT_EQ(out.srcIp, "");
}

TEST(IPHandler, PassesCorrectOffsetToNextHandler) {
    auto frame = buildTCPFrame("1.1.1.1", "2.2.2.2", 9999, 22);
    auto tcp = std::make_unique<TCPHandler>();
    IPHandler ip;
    ip.setNext(std::move(tcp));
    PacketInfo out;
    ip.handle(frame.data(), static_cast<int>(frame.size()), 14, out);
    EXPECT_EQ(out.protocol, "TCP");
    EXPECT_EQ(out.dstPort,  22u);
}

TEST(IPHandler, LoopbackAddresses) {
    auto frame = buildTCPFrame("127.0.0.1", "127.0.0.1", 3000, 3001);
    IPHandler ip;
    PacketInfo out;
    ip.handle(frame.data(), static_cast<int>(frame.size()), 14, out);
    EXPECT_EQ(out.srcIp, "127.0.0.1");
    EXPECT_EQ(out.dstIp, "127.0.0.1");
}

TEST(TCPHandler, SetsProtocolToTCP) {
    tcphdr hdr{}; hdr.source = htons(1); hdr.dest = htons(2);
    TCPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.protocol, "TCP");
}

TEST(TCPHandler, ExtractsSrcPort) {
    tcphdr hdr{}; hdr.source = htons(54000); hdr.dest = htons(443);
    TCPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.srcPort, 54000u);
}

TEST(TCPHandler, ExtractsDstPort) {
    tcphdr hdr{}; hdr.source = htons(1024); hdr.dest = htons(80);
    TCPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.dstPort, 80u);
}

TEST(TCPHandler, TruncatedHeaderDoesNotCrash) {
    uint8_t tiny[3] = {};
    TCPHandler h;
    PacketInfo out;
    h.handle(tiny, sizeof(tiny), 0, out);
    EXPECT_EQ(out.protocol, "");
}

TEST(TCPHandler, MaxPortValue) {
    tcphdr hdr{}; hdr.source = htons(65535); hdr.dest = htons(65535);
    TCPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.srcPort, 65535u);
    EXPECT_EQ(out.dstPort, 65535u);
}

TEST(TCPHandler, ZeroPortValue) {
    tcphdr hdr{}; hdr.source = 0; hdr.dest = 0;
    TCPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.srcPort, 0u);
    EXPECT_EQ(out.dstPort, 0u);
}


TEST(UDPHandler, SetsProtocolToUDP) {
    udphdr hdr{}; hdr.source = htons(1); hdr.dest = htons(2);
    UDPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.protocol, "UDP");
}

TEST(UDPHandler, ExtractsSrcPort) {
    udphdr hdr{}; hdr.source = htons(5353); hdr.dest = htons(53);
    UDPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.srcPort, 5353u);
}

TEST(UDPHandler, ExtractsDstPort) {
    udphdr hdr{}; hdr.source = htons(9000); hdr.dest = htons(123);
    UDPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.dstPort, 123u);
}

TEST(UDPHandler, TruncatedHeaderDoesNotCrash) {
    uint8_t tiny[2] = {};
    UDPHandler h;
    PacketInfo out;
    h.handle(tiny, sizeof(tiny), 0, out);
    EXPECT_EQ(out.protocol, "");
}

TEST(UDPHandler, MaxPortValue) {
    udphdr hdr{}; hdr.source = htons(65535); hdr.dest = htons(65535);
    UDPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.srcPort, 65535u);
    EXPECT_EQ(out.dstPort, 65535u);
}

TEST(UDPHandler, WellKnownDNSPort) {
    udphdr hdr{}; hdr.source = htons(12345); hdr.dest = htons(53);
    UDPHandler h;
    PacketInfo out;
    h.handle(reinterpret_cast<const uint8_t*>(&hdr), sizeof(hdr), 0, out);
    EXPECT_EQ(out.dstPort, 53u);
}
