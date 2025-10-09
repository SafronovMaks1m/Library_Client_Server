#pragma once
#include <memory>
#include "Message.h"

class HandlerMessage {
    virtual void handler(std::unique_ptr<BaseMessage>& msg) = 0;
};  