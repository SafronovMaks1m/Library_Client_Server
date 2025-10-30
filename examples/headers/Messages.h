#pragma once
#include "Message.h"
#include "enums.h"


struct PingMessage : Message<Messages::Ping> { 
    uint64_t timestamp;
    PingMessage(const uint64_t& ts): timestamp(ts){}
};

struct PongMessage : Message<Messages::Pong> { 
    uint64_t timestamp; 
    PongMessage(const uint64_t& ts) : timestamp(ts) {}
};

struct DisconnectMessage : Message<Messages::Disconnect> {
    std::string reason;
    DisconnectMessage(const std::string& reason_msg = "") : reason(reason_msg) {}
};

struct ConnectionAcceptMessage : Message<Messages::Connection> {
    bool accepted;
    std::string reason;
    ConnectionAcceptMessage(const bool& accept, const std::string& reason_msg = ""): accepted(accept), reason(reason_msg){}
};