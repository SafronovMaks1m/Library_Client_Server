#include <string>
#include <algorithm>
#include <chrono>
#include <iostream>
#include "Server.h"
#include "server_create_handlers.h"

void handler_ping(Server& server, PingMessage& msg, Connection& connection) {
    PongMessage pong(msg.timestamp);
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
    std::cout << "Client disconnected, reason: " << msg.reason << std::endl;
    log(out);
}

void handler_simple(Server& server, SimpleMessage& msg, Connection& connection){
    std::string out = msg.text;
    std::cout << msg.text << std::endl;
    log(out);
}

void handler_forwarding(Server& server, ForwardingMessage& msg, Connection& connection){
    std::string out = msg.text;
    std::cout << msg.text << std::endl;
    std::unique_ptr<BaseMessage> reply = std::make_unique<SimpleMessage>("Accepted");
    log(out);
    server.send_message(std::move(reply), connection);
}