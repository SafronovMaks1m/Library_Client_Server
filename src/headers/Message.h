#pragma once
#include <string>

struct BaseMessage{
    public:
        using MessageType = uint16_t;
        virtual ~BaseMessage() = default;
        virtual MessageType type_name() const = 0;
};