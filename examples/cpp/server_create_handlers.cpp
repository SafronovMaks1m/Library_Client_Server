#include <string>
#include <algorithm>
#include <chrono>
#include <iostream>
#include "Server.h"
#include "../headers/server_create_handlers.h"
#include "HandlerMessageServer.h"

void handler_ping(Server& server, PingMessage& msg, Connection& connection) {
    PongMessage pong(msg.timestamp);
    server.send_message(pong, connection);
    std::string out = "Send a pong via the send_message method on the passed connection, " 
        + std::to_string(msg.timestamp) + " ms";
    std::cout << out;
    log(out);
}

void handler_pong(Server& server, PongMessage& msg, Connection& connection) {
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg.timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::string out = "Ping = " + std::to_string(ping_time.count()) + " ms";
    std::cout << out << std::endl;
    log(out);
}

void handler_disconnect(Server& server, DisconnectMessage& msg, Connection& connection){
    std::string out = "Client disconnected, reason: " + msg.reason;
    for (auto it = server.setConnections().begin(); it != server.setConnections().end(); it++) {
        if (it->get() == &connection) {
            server.setConnections().erase(it);
            break;                         
        }
    }
    std::cout << "Client disconnected, reason: " << msg.reason << std::endl;
    log(out);
}

void reg(Server& server){
    server.handler->register_handler<PingMessage>(1, handler_ping);
    server.handler->register_handler<PongMessage>(2, handler_pong);
    server.handler->register_handler<DisconnectMessage>(3, handler_disconnect);
}