#pragma once
#include <string>
#include "Connection.h"

struct BaseMessage{
    protected:
        bool priority = false;
        virtual ~BaseMessage() = default;
    public:
        bool get_priority() const{
            return priority;
        }
        virtual std::string type_name() const = 0;
};

struct ConnectionRequestMessage : BaseMessage {
    std::string client_name;
    ConnectionRequestMessage(const std::string& name): client_name(name) {priority = true;}
    std::string type_name() const {return "ConnectionRequestMessage";}
};


struct ConnectionAcceptMessage : BaseMessage {
    bool accepted;
    std::string reason;
    ConnectionAcceptMessage(){priority = true;}
    std::string type_name() const {return "ConnectionAcceptMessage";}
};

struct PingMessage : BaseMessage { 
    uint64_t timestamp;
    std::string client_name;
    PingMessage(const std::string& name, const uint64_t& ts): client_name(name), timestamp(ts){
        priority = true;
    }
    std::string type_name() const {return "PingMessage";}
};

struct PongMessage : BaseMessage { 
    uint64_t timestamp; 
    PongMessage(const uint64_t& ts) : timestamp(ts) {
        priority = true;
    }
    std::string type_name() const {return "PongMessage";}
};

// struct DisconnectMessage : BaseMessage {
//     std::string reason;     
//     uint64_t timestamp;    

// };