#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <string>
#include "Message.h"
#include "Serealize.h"
#include "Server.h"
#include "Connection.h"

class HandlerMessageServer {
private:
    Server& _server;
    
    using MessageType = std::uint16_t;
    std::unordered_map<MessageType, std::function<void(BaseMessage&, Connection&)>> handlers;

public:
    template <class T>
    void register_handler(std::function<void(Server&, T&, Connection&)> func) {
        MessageType id = static_cast<MessageType>(T::value);
        
        if (handlers.find(id) != handlers.end()) {
            throw std::logic_error("Handler for this type_name already exists");
        }
        
        handlers[id] = [this, func](BaseMessage& msg, Connection& connection) {
            func(_server, static_cast<T&>(msg), connection);
        };
        
        Serealize::register_deserializer<T>(id);
    }

    HandlerMessageServer(Server& server);

    void handler(std::unique_ptr<BaseMessage>&& msg, Connection& connection);
};