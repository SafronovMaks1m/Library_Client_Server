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
#include "Server.h"
#include "Connection.h"

class HandlerMessageServer {
private:
    Server& _server;

    std::string _lastLog;
    void log(const std::string& message) { _lastLog = message; }
public:
    HandlerMessageServer(Server& server);
    void handler(std::unique_ptr<BaseMessage>&& msg, const std::shared_ptr<Connection>& connection);
    void handler_ping(PingMessage* msg, const std::shared_ptr<Connection>& connection);
    void handler_pong(PongMessage* msg);
    void handler_disconnect(DisconnectMessage* msg, const std::shared_ptr<Connection>& connection);
    const std::string& getLastLog() const { return _lastLog; }
};