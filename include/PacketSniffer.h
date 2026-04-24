#pragma once
#include "CaptureEngine.h"
#include "ProtocolParser.h"
#include "StatisticsCollector.h"
#include "OutputRenderer.h"
#include <memory>

class PacketSniffer {
public:
    PacketSniffer();
    void run();
    void stop();

private:
    CaptureEngine                        engine_;
    ProtocolParser                       parser_;
    std::shared_ptr<StatisticsCollector> stats_;
    std::shared_ptr<OutputRenderer>      renderer_;

    static PacketSniffer* instance_;
    static void sigHandler(int sig);
};
