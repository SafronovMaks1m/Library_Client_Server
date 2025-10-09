#pragma once
#include "memory"
#include "Message.h"

struct CompareMessage {
    bool operator()(const std::unique_ptr<BaseMessage>& a, const std::unique_ptr<BaseMessage>& b) const {
        return a->get_priority() < b->get_priority();
    }
};