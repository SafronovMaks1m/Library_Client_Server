#pragma once
#include <memory>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <iostream>
#include <string>
#include "Message.h"
#include "Client.h"

class HandlerMessageClient {
private:
    Client& _client;

    using MessageType = std::uint16_t;
    std::unordered_map<MessageType, std::function<void(BaseMessage& msg)>> handlers;
public:
    template <class T>
    void register_handler(MessageType type_name, std::function<void(Client&, T&)> func) {
        if (handlers.find(type_name) != handlers.end()) {
            throw std::logic_error("Handler for this type_name already exists");
        }
        handlers[type_name] = [this, func](BaseMessage& msg) {
            func(_client, static_cast<T&>(msg));
        };
    }
    HandlerMessageClient(Client& client);
    void handler(std::unique_ptr<BaseMessage>&& msg);
};