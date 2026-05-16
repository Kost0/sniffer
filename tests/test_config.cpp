#include <gtest/gtest.h>
#include "Config.h"
#include <getopt.h>

static void resetConfig() {
    optind = 1;
    auto& c = Config::getInstance();
    c.interface = "";
    c.filter    = "";
    c.count     = 0;
}

TEST(Config, SingletonReturnsSameInstance) {
    auto& a = Config::getInstance();
    auto& b = Config::getInstance();
    EXPECT_EQ(&a, &b);
}

TEST(Config, DefaultValuesAreEmpty) {
    resetConfig();
    auto& c = Config::getInstance();
    EXPECT_EQ(c.interface, "");
    EXPECT_EQ(c.filter,    "");
    EXPECT_EQ(c.count,     0);
}

TEST(Config, ParsesInterfaceFlag) {
    resetConfig();
    char* argv[] = { (char*)"sniffer", (char*)"-i", (char*)"eth0" };
    Config::getInstance().parse(3, argv);
    EXPECT_EQ(Config::getInstance().interface, "eth0");
}

TEST(Config, ParsesFilterFlag) {
    resetConfig();
    char* argv[] = { (char*)"sniffer", (char*)"-f", (char*)"tcp port 80" };
    Config::getInstance().parse(3, argv);
    EXPECT_EQ(Config::getInstance().filter, "tcp port 80");
}

TEST(Config, ParsesCountFlag) {
    resetConfig();
    char* argv[] = { (char*)"sniffer", (char*)"-n", (char*)"100" };
    Config::getInstance().parse(3, argv);
    EXPECT_EQ(Config::getInstance().count, 100);
}

TEST(Config, ParsesMultipleFlagsTogether) {
    resetConfig();
    char* argv[] = { (char*)"sniffer",
                     (char*)"-i", (char*)"wlan0",
                     (char*)"-f", (char*)"udp",
                     (char*)"-n", (char*)"50",};
    Config::getInstance().parse(7, argv);
    auto& c = Config::getInstance();
    EXPECT_EQ(c.interface, "wlan0");
    EXPECT_EQ(c.filter,    "udp");
    EXPECT_EQ(c.count,     50);
}

TEST(Config, ZeroCountMeansUnlimited) {
    resetConfig();
    char* argv[] = { (char*)"sniffer", (char*)"-n", (char*)"0" };
    Config::getInstance().parse(3, argv);
    EXPECT_EQ(Config::getInstance().count, 0);
}

TEST(Config, NoFlagsLeavesDefaults) {
    resetConfig();
    char* argv[] = { (char*)"sniffer" };
    Config::getInstance().parse(1, argv);
    auto& c = Config::getInstance();
    EXPECT_EQ(c.interface, "");
    EXPECT_EQ(c.count,     0);
}

TEST(Config, InterfaceLoopback) {
    resetConfig();
    char* argv[] = { (char*)"sniffer", (char*)"-i", (char*)"lo" };
    Config::getInstance().parse(3, argv);
    EXPECT_EQ(Config::getInstance().interface, "lo");
}
