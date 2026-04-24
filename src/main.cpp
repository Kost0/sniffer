#include "Config.h"
#include "PacketSniffer.h"

int main(int argc, char** argv) {
    Config::getInstance().parse(argc, argv);
    PacketSniffer sniffer;
    sniffer.run();
    return 0;
}
