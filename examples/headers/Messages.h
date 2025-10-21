#pragma once
#include "Message.h"

struct PingMessage : BaseMessage { 
    uint64_t timestamp;
    PingMessage(const uint64_t& ts): timestamp(ts){}
    MessageType type_name() const {return 1;}
};

struct PongMessage : BaseMessage { 
    uint64_t timestamp; 
    PongMessage(const uint64_t& ts) : timestamp(ts) {}
    MessageType type_name() const {return 2;}
};

struct DisconnectMessage : BaseMessage {
    std::string reason;
    DisconnectMessage(const std::string& reason_msg = "") : reason(reason_msg) {}
    MessageType type_name() const {return 3;}
};

struct ConnectionAcceptMessage : BaseMessage {
    bool accepted;
    std::string reason;
    ConnectionAcceptMessage(const bool& accept, const std::string& reason_msg = ""): accepted(accept), reason(reason_msg){}
    MessageType type_name() const {return 4;}
};