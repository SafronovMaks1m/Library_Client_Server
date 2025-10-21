#pragma once
#include <memory>
#include "Connection.h" 
#include "Message.h"

class HandlerMessage {
    public:
        virtual void handler(std::unique_ptr<BaseMessage>&& msg, const std::unique_ptr<Connection>& connection = nullptr) = 0;
};