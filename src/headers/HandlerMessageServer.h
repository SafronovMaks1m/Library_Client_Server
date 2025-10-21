#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <string>
#include "Message.h"
#include "Server.h"
#include "Connection.h"

class HandlerMessageServer {
private:
    Server& _server;
    
    using MessageType = uint16_t;
    std::unordered_map<MessageType, std::function<void(BaseMessage& msg, Connection& connection)>> handlers;

public:
    template <class T>
    void register_handler(MessageType type_name, std::function<void(Server&, T&, Connection&)> func) {
        if (handlers.find(type_name) != handlers.end()) {
            throw std::logic_error("Handler for this type_name already exists");
        }
        handlers[type_name] = [this, func](BaseMessage& msg, Connection& connection) {
            func(_server, static_cast<T&>(msg), connection);
        };
    }
    HandlerMessageServer(Server& server);
    void handler(std::unique_ptr<BaseMessage>&& msg, Connection& connection);
};