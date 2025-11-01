#pragma once
#include <memory>
#include "vector"
#include "Message.h"
#include "stdint.h"

class ISerealize{
    virtual std::vector<uint8_t> serialize(std::unique_ptr<BaseMessage>& msg) = 0;

    virtual std::unique_ptr<BaseMessage> deserialize(std::vector<uint8_t>& data) = 0;
};