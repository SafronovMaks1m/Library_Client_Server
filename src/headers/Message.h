#pragma once
#include <cstdint>
#include <typeinfo>

template<auto V>
struct Tag {
    static constexpr auto value = V;  
};

struct BaseMessage {
    virtual ~BaseMessage() = default;
    virtual uint16_t getType() const = 0;
};

template <auto T>
struct Message : public BaseMessage, public Tag<T> {
    uint16_t getType() const { 
        return static_cast<uint16_t>(T); 
    }
};