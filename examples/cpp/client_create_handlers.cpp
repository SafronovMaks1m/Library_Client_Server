#include <iostream>
#include <chrono>
#include "client_create_handlers.h"

void handler_connect(Client& client, ConnectionAcceptMessage& msg){
    if (msg.accepted) {
        std::cout << "Connection successful";
        log("Connection successful");
    } else {
        std::cout << "Connection failed. Error: " << msg.reason;
        log("Connection failed. Error: " + msg.reason);
    }
}

void handler_ping(Client& client, PingMessage& msg){
    std::string out = "send pong to the server with " + std::to_string(msg.timestamp) + " ms"; 
    std::cout << out << std::endl;
    log(out);
}

void handler_pong(Client& client, PongMessage& msg){
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg.timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::string out = "Ping = " + std::to_string(ping_time.count()) + " ms";
    std::cout << out << "ms";
    log(out);

}
void handler_disconnect(Client& client, DisconnectMessage& msg){
    std::string out = "Disconnected from server. Reason: " + msg.reason;
    std::cout << out << std::endl;
    log(out);
}

void handler_simple(Client& server, SimpleMessage& msg){
    std::string out = msg.text;
    std::cout << msg.text << std::endl;
    log(out);
}