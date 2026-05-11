#include <gtest/gtest.h>
#include "ProtocolParserFactory.h"
#include "IProtocolParser.h"
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <cstring>


TEST(ProtocolParserFactory, Protocol6ReturnsTCPParser) {
    auto parser = ProtocolParserFactory::create(6);
    ASSERT_NE(parser, nullptr);
    tcphdr hdr{};
    hdr.source = htons(12345);
    hdr.dest   = htons(80);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.protocol, "TCP");
}

TEST(ProtocolParserFactory, Protocol17ReturnsUDPParser) {
    auto parser = ProtocolParserFactory::create(17);
    ASSERT_NE(parser, nullptr);
    udphdr hdr{};
    hdr.source = htons(5353);
    hdr.dest   = htons(53);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.protocol, "UDP");
}

TEST(ProtocolParserFactory, UnknownProtocolReturnsUnsupportedParser) {
    auto parser = ProtocolParserFactory::create(1);
    ASSERT_NE(parser, nullptr);
    uint8_t dummy[8] = {};
    auto pkt = parser->parse(dummy, sizeof(dummy));
    EXPECT_EQ(pkt.protocol, "");
}

TEST(ProtocolParserFactory, Protocol0ReturnsUnsupported) {
    auto parser = ProtocolParserFactory::create(0);
    uint8_t dummy[8] = {};
    auto pkt = parser->parse(dummy, sizeof(dummy));
    EXPECT_EQ(pkt.protocol, "");
}

TEST(ProtocolParserFactory, Protocol255ReturnsUnsupported) {
    auto parser = ProtocolParserFactory::create(255);
    uint8_t dummy[8] = {};
    auto pkt = parser->parse(dummy, sizeof(dummy));
    EXPECT_EQ(pkt.protocol, "");
}


TEST(ProtocolParserFactory, TCPParserExtractsSrcPort) {
    auto parser = ProtocolParserFactory::create(6);
    tcphdr hdr{};
    hdr.source = htons(54321);
    hdr.dest   = htons(443);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.srcPort, 54321u);
}

TEST(ProtocolParserFactory, TCPParserExtractsDstPort) {
    auto parser = ProtocolParserFactory::create(6);
    tcphdr hdr{};
    hdr.source = htons(12345);
    hdr.dest   = htons(8080);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.dstPort, 8080u);
}

TEST(ProtocolParserFactory, TCPParserHandlesTruncatedHeader) {
    auto parser = ProtocolParserFactory::create(6);
    uint8_t tiny[2] = {0x00, 0x50};
    auto pkt = parser->parse(tiny, sizeof(tiny));
    EXPECT_EQ(pkt.protocol, "TCP");
    EXPECT_EQ(pkt.srcPort,  0u);
    EXPECT_EQ(pkt.dstPort,  0u);
}

TEST(ProtocolParserFactory, TCPParserWellKnownPorts) {
    auto parser = ProtocolParserFactory::create(6);
    tcphdr hdr{};
    hdr.source = htons(1024);
    hdr.dest   = htons(22);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.dstPort, 22u);
}


TEST(ProtocolParserFactory, UDPParserExtractsSrcPort) {
    auto parser = ProtocolParserFactory::create(17);
    udphdr hdr{};
    hdr.source = htons(5353);
    hdr.dest   = htons(53);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.srcPort, 5353u);
}

TEST(ProtocolParserFactory, UDPParserExtractsDstPort) {
    auto parser = ProtocolParserFactory::create(17);
    udphdr hdr{};
    hdr.source = htons(9999);
    hdr.dest   = htons(123);
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.dstPort, 123u);
}

TEST(ProtocolParserFactory, UDPParserHandlesTruncatedHeader) {
    auto parser = ProtocolParserFactory::create(17);
    uint8_t tiny[3] = {};
    auto pkt = parser->parse(tiny, sizeof(tiny));
    EXPECT_EQ(pkt.protocol, "UDP");
    EXPECT_EQ(pkt.srcPort,  0u);
    EXPECT_EQ(pkt.dstPort,  0u);
}

TEST(ProtocolParserFactory, UDPParserZeroPorts) {
    auto parser = ProtocolParserFactory::create(17);
    udphdr hdr{};
    hdr.source = 0;
    hdr.dest   = 0;
    auto pkt = parser->parse(reinterpret_cast<const uint8_t*>(&hdr),
                              sizeof(hdr));
    EXPECT_EQ(pkt.srcPort, 0u);
    EXPECT_EQ(pkt.dstPort, 0u);
}
