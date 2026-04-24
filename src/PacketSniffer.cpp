#include "PacketSniffer.h"
#include "Config.h"
#include <cstdio>
#include <csignal>
#include <pcap.h>

PacketSniffer* PacketSniffer::instance_ = nullptr;

void PacketSniffer::sigHandler(int /*sig*/) {
    if (instance_) instance_->stop();
}

PacketSniffer::PacketSniffer()
    : stats_(std::make_shared<StatisticsCollector>())
    , renderer_(std::make_shared<OutputRenderer>())
{
    instance_ = this;
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
}

void PacketSniffer::run() {
    const auto& cfg = Config::getInstance();

    // Observer: subscribe stats collector and renderer
    engine_.addObserver(stats_);
    engine_.addObserver(renderer_);

    // Must specify an interface
    if (cfg.interface.empty()) {
        pcap_if_t* devs = nullptr;
        char errbuf[PCAP_ERRBUF_SIZE];
        if (pcap_findalldevs(&devs, errbuf) == 0) {
            std::printf("Available interfaces:\n");
            for (pcap_if_t* d = devs; d; d = d->next)
                std::printf("  %s\n", d->name);
            pcap_freealldevs(devs);
        }
        std::printf("Use -i <interface>. Run with -h for help.\n");
        return;
    }

    if (!engine_.open(cfg.interface)) return;

    if (!cfg.filter.empty())
        engine_.setFilter(cfg.filter);

    std::printf("[*] Capturing... Press Ctrl+C to stop.\n\n");
    engine_.startCapture(cfg.count);

    renderer_->printSummary(*stats_);
}

void PacketSniffer::stop() {
    engine_.stop();
}
