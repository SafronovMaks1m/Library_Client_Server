#include "HandlerMessageServer.h"
#include <algorithm>

void HandlerMessageServer::handler(std::unique_ptr<BaseMessage>& msg) {
    auto it = handlers.find(msg->type_name());
    if (it != handlers.end()) {
        it->second(*msg);
    }
    else if (auto* mes = dynamic_cast<ConnectionRequestMessage*>(msg.get())) {
        handler(mes);
    }
    else if (auto* mes = dynamic_cast<PingMessage*>(msg.get())) {
        handler(mes);
    }
    else if (auto* mes = dynamic_cast<PongMessage*>(msg.get())) {
        handler(mes);
    }
    else {
        std::cout << "no handler for the message";
    }
}

void HandlerMessageServer::handler(ConnectionRequestMessage* msg) {
    auto& connections = _server._connections;
    if (!connections.empty()) {
        auto last_it = std::prev(connections.end());
        auto value = std::move(last_it->second);
        connections.erase(last_it); 
        connections[msg->client_name] = std::move(value);
    }
}

void HandlerMessageServer::handler(PingMessage* msg) {
    PongMessage pong(msg->timestamp);
    auto& connect = _server._connections[msg->client_name];
    std::cout << "send pong using the found connect";
}

void HandlerMessageServer::handler(PongMessage* msg) {
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg->timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::cout << "Ping = " << ping_time.count() << "ms";
}