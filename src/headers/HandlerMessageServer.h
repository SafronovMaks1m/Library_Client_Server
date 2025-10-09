#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <iostream>
#include "Message.h"
#include <string>
#include "HandlerMessage.h"
#include "Server.h"

class HandlerMessageServer : public HandlerMessage {
private:
    Server& _server;
    std::unordered_map<std::string, std::function<void(BaseMessage& msg)>> handlers;

    template <class T>
    void register_handler(std::string name, std::function<void(T&)> func) {
        handlers[name] = [func](BaseMessage& msg){ 
            func(static_cast<T&>(msg));
        };
    }

    void handler(std::unique_ptr<BaseMessage>& msg);

public:
    HandlerMessageServer(Server& server);
    
    void handler(ConnectionRequestMessage* msg);
    void handler(PingMessage* msg);
    void handler(PongMessage* msg);
};