
#include "HandlerMessageClient.h"

void HandlerMessageClient::handler(std::unique_ptr<BaseMessage>& msg) {
    auto it = handlers.find(msg->type_name()); 
    if (it != handlers.end()) {
        it->second(*(msg.get()));
    }
    else if (auto* mes = dynamic_cast<ConnectionAcceptMessage*>(msg.get())) {
        handler(mes);
    }
    else if (auto* mes = dynamic_cast<PingMessage*>(msg.get())) {
        handler(mes);
    }
    else if (auto* mes = dynamic_cast<PongMessage*>(msg.get())) {
        handler(mes);
    }
    else {
        std::cout << "No handler for message";
    }
}

void HandlerMessageClient::handler(ConnectionAcceptMessage* msg) {
    if (msg->accepted == true) {
        std::cout << "Connection successful";
    } else {
        std::cout << "Connection failed. Error: " << msg->reason;
    }
}

void HandlerMessageClient::handler(PingMessage* msg) {
    PongMessage pong(msg->timestamp);
    std::cout << "send pong to the server" << std::endl;
}

void HandlerMessageClient::handler(PongMessage* msg) {
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg->timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::cout << "Ping = " << ping_time.count() << "ms";
}