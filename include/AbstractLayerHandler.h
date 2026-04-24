#pragma once
#include "PacketInfo.h"
#include <memory>
#include <cstdint>

class AbstractLayerHandler {
public:
    virtual ~AbstractLayerHandler() = default;

    AbstractLayerHandler* setNext(std::unique_ptr<AbstractLayerHandler> next) {
        next_ = std::move(next);
        return next_.get();
    }

    virtual void handle(const uint8_t* data, int len,
                        int offset, PacketInfo& out) = 0;

protected:
    std::unique_ptr<AbstractLayerHandler> next_;
};
