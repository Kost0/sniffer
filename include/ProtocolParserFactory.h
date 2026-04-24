#pragma once
#include "IProtocolParser.h"
#include <memory>
#include <cstdint>

struct ProtocolParserFactory {
    static std::unique_ptr<IProtocolParser> create(uint8_t protocol);
};
