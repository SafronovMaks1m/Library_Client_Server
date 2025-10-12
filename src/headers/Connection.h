#pragma once
#include "Message.h"
#include <memory>

class Connection {
    public:
        void send(std::unique_ptr<BaseMessage>&& msg);
        void recv();
};