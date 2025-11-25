#pragma once
#include "Message.h"
#include "enums.h"


struct PingMessage : Message<Messages::Ping> { 
    uint64_t timestamp;
    PingMessage() = default;  
    PingMessage(const uint64_t& ts): timestamp(ts){}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & timestamp;
    };
};

struct PongMessage : Message<Messages::Pong> { 
    uint64_t timestamp; 
    PongMessage() = default;
    PongMessage(const uint64_t& ts) : timestamp(ts) {}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & timestamp;
    };
};

struct DisconnectMessage : Message<Messages::Disconnect> {
    std::string reason;
    DisconnectMessage() = default;
    DisconnectMessage(const std::string& reason_msg) : reason(reason_msg) {}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & reason;
    };
};

struct ConnectionAcceptMessage : Message<Messages::Connection> {
    bool accepted;
    std::string reason;
    ConnectionAcceptMessage() = default;
    ConnectionAcceptMessage(const bool& accept, const std::string& reason_msg = ""): accepted(accept), reason(reason_msg){}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & accepted;
        ar & reason;
    };
};

struct SimpleMessage : Message<Messages::Simple> {
    std::string text;
    SimpleMessage() = default;
    SimpleMessage(const std::string& text): text(text){}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & text;
    };
};

struct ForwardingMessage : Message<Messages::Forwarding> {
    std::string text;
    ForwardingMessage() = default;
    ForwardingMessage(const std::string& text): text(text){}

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version){
        ar & text;
    };
};