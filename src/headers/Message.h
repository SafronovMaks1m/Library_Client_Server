#pragma once
#include <string>

struct BaseMessage{
    public:
        virtual ~BaseMessage() = default;
};

struct ConnectionAcceptMessage : BaseMessage {
    bool accepted;
    std::string reason;
    ConnectionAcceptMessage(const bool& accept, const std::string& reason_msg = ""): accepted(accept), reason(reason_msg){}
};

struct PingMessage : BaseMessage { 
    uint64_t timestamp;
    PingMessage(const uint64_t& ts): timestamp(ts){}
};

struct PongMessage : BaseMessage { 
    uint64_t timestamp; 
    PongMessage(const uint64_t& ts) : timestamp(ts) {}
};

struct DisconnectMessage : BaseMessage {
    std::string reason;
    
    DisconnectMessage(const std::string& reason_msg = "") : reason(reason_msg) {}
};