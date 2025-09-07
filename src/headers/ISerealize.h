#include "vector"
#include "Message.h"
#include "stdint.h"
#pragma once

class ISerealize{
    virtual std::vector<uint8_t> serialize(Message& msg) = 0;

    virtual Message deserialize(std::vector<uint8_t>& data) = 0;
};