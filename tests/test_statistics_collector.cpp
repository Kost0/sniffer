#include <gtest/gtest.h>
#include "StatisticsCollector.h"
#include "PacketInfo.h"

static PacketInfo makePkt(const std::string& proto,
                           const std::string& src,
                           const std::string& dst,
                           int len) {
    PacketInfo p;
    p.protocol = proto;
    p.srcIp    = src;
    p.dstIp    = dst;
    p.length   = len;
    return p;
}


TEST(StatisticsCollector, InitialCountersAreZero) {
    StatisticsCollector sc;
    EXPECT_EQ(sc.totalPackets(), 0u);
    EXPECT_EQ(sc.totalBytes(),   0u);
}

TEST(StatisticsCollector, SinglePacketIncreasesCounterByOne) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    EXPECT_EQ(sc.totalPackets(), 1u);
}

TEST(StatisticsCollector, SinglePacketAddsCorrectBytes) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 512));
    EXPECT_EQ(sc.totalBytes(), 512u);
}

TEST(StatisticsCollector, MultiplePacketsAccumulateCorrectly) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    sc.onPacket(makePkt("UDP", "3.3.3.3", "4.4.4.4", 200));
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 300));
    EXPECT_EQ(sc.totalPackets(), 3u);
    EXPECT_EQ(sc.totalBytes(),   600u);
}

TEST(StatisticsCollector, ZeroLengthPacketCountedButNoBytes) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 0));
    EXPECT_EQ(sc.totalPackets(), 1u);
    EXPECT_EQ(sc.totalBytes(),   0u);
}

TEST(StatisticsCollector, LargeNumberOfPackets) {
    StatisticsCollector sc;
    for (int i = 0; i < 10000; ++i)
        sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 64));
    EXPECT_EQ(sc.totalPackets(), 10000u);
    EXPECT_EQ(sc.totalBytes(),   640000u);
}


TEST(StatisticsCollector, ProtocolPacketCountIsCorrect) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 200));
    sc.onPacket(makePkt("UDP", "3.3.3.3", "4.4.4.4", 50));
    EXPECT_EQ(sc.packetsForProtocol("TCP"), 2u);
    EXPECT_EQ(sc.packetsForProtocol("UDP"), 1u);
}

TEST(StatisticsCollector, ProtocolBytesAccumulateCorrectly) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("UDP", "1.1.1.1", "2.2.2.2", 300));
    sc.onPacket(makePkt("UDP", "1.1.1.1", "2.2.2.2", 700));
    EXPECT_EQ(sc.bytesForProtocol("UDP"), 1000u);
}

TEST(StatisticsCollector, UnknownProtocolReturnsZero) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    EXPECT_EQ(sc.packetsForProtocol("UDP"), 0u);
    EXPECT_EQ(sc.bytesForProtocol("ICMP"),  0u);
}

TEST(StatisticsCollector, HasProtocolReturnsTrueAfterPacket) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    EXPECT_TRUE(sc.hasProtocol("TCP"));
    EXPECT_FALSE(sc.hasProtocol("UDP"));
}

TEST(StatisticsCollector, TwoProtocolsTrackedIndependently) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    sc.onPacket(makePkt("UDP", "3.3.3.3", "4.4.4.4", 200));
    EXPECT_EQ(sc.packetsForProtocol("TCP"), 1u);
    EXPECT_EQ(sc.packetsForProtocol("UDP"), 1u);
    EXPECT_EQ(sc.bytesForProtocol("TCP"),   100u);
    EXPECT_EQ(sc.bytesForProtocol("UDP"),   200u);
}


TEST(StatisticsCollector, SrcIpTrafficAccumulated) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "10.0.0.1", "10.0.0.2", 400));
    EXPECT_EQ(sc.bytesForIp("10.0.0.1"), 400u);
}

TEST(StatisticsCollector, DstIpTrafficAccumulated) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "10.0.0.1", "10.0.0.2", 400));
    EXPECT_EQ(sc.bytesForIp("10.0.0.2"), 400u);
}

TEST(StatisticsCollector, IpAppearsAsBothSrcAndDst) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "10.0.0.1", "10.0.0.2", 100));
    sc.onPacket(makePkt("TCP", "10.0.0.2", "10.0.0.1", 200));
    EXPECT_EQ(sc.bytesForIp("10.0.0.1"), 300u);
}

TEST(StatisticsCollector, EmptyIpNotTracked) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "", "", 100));
    EXPECT_EQ(sc.bytesForIp(""), 0u);
}

TEST(StatisticsCollector, MultipleIpsTrackedSeparately) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    sc.onPacket(makePkt("TCP", "3.3.3.3", "4.4.4.4", 500));
    EXPECT_EQ(sc.bytesForIp("1.1.1.1"), 100u);
    EXPECT_EQ(sc.bytesForIp("3.3.3.3"), 500u);
}


TEST(StatisticsCollector, ResetClearsAllCounters) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 100));
    sc.reset();
    EXPECT_EQ(sc.totalPackets(), 0u);
    EXPECT_EQ(sc.totalBytes(),   0u);
    EXPECT_FALSE(sc.hasProtocol("TCP"));
    EXPECT_EQ(sc.bytesForIp("1.1.1.1"), 0u);
}

TEST(StatisticsCollector, WorksCorrectlyAfterReset) {
    StatisticsCollector sc;
    sc.onPacket(makePkt("TCP", "1.1.1.1", "2.2.2.2", 999));
    sc.reset();
    sc.onPacket(makePkt("UDP", "5.5.5.5", "6.6.6.6", 42));
    EXPECT_EQ(sc.totalPackets(), 1u);
    EXPECT_EQ(sc.totalBytes(),   42u);
    EXPECT_EQ(sc.packetsForProtocol("UDP"), 1u);
    EXPECT_FALSE(sc.hasProtocol("TCP"));
}
