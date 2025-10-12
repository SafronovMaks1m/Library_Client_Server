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

class HandlerMessageClient{
private:
    Client& _client;

    std::string _lastLog;
    void log(const std::string& message) { _lastLog = message; }
public:
    HandlerMessageClient(Client& client);
    void handler(std::unique_ptr<BaseMessage>&& msg);
    void handler_connect(ConnectionAcceptMessage* msg);
    void handler_ping(PingMessage* msg);
    void handler_pong(PongMessage* msg);
    void handler_disconnect(DisconnectMessage* msg);
    const std::string& getLastLog() const { return _lastLog; }
};