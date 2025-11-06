#pragma once
#include <cstdint>
#include <typeinfo>
#include <boost/serialization/access.hpp>

template<auto V>
struct Tag {
    static constexpr auto value = V;  
};

struct BaseMessage {
    virtual ~BaseMessage() = default;
    virtual uint16_t getType() const = 0;

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){};
};

template <auto T>
struct Message : public BaseMessage, public Tag<T> {
    friend class boost::serialization::access;
    Message() = default;

    uint16_t getType() const { 
        return static_cast<uint16_t>(T); 
    }
};