#include "HandlerMessageClient.h"

HandlerMessageClient::HandlerMessageClient(Client& client): _client(client){}

void HandlerMessageClient::handler(std::unique_ptr<BaseMessage>&& msg) {
    if (auto* mes = dynamic_cast<ConnectionAcceptMessage*>(msg.get())) {
        handler_connect(mes);
    }
    else if (auto* mes = dynamic_cast<PingMessage*>(msg.get())) {
        handler_ping(mes);
    }
    else if (auto* mes = dynamic_cast<PongMessage*>(msg.get())) {
        handler_pong(mes);
    }
    else if (auto* mes = dynamic_cast<DisconnectMessage*>(msg.get())){
        handler_disconnect(mes);
    }
    else {
        std::cout << "no handler for the message";
        log("no handler for the message");
    }
}

void HandlerMessageClient::handler_connect(ConnectionAcceptMessage* msg) {
    if (msg->accepted) {
        std::cout << "Connection successful";
        log("Connection successful");
    } else {
        std::cout << "Connection failed. Error: " << msg->reason;
        log("Connection failed. Error: " + msg->reason);
    }
}

void HandlerMessageClient::handler_ping(PingMessage* msg) {
    _client._connection = std::make_shared<Connection>(); // потом уберу, просто чтобы правильно работали тесты
    _client._connection->send(std::make_unique<PongMessage>(msg->timestamp));
    std::string out = "send pong to the server with " + std::to_string(msg->timestamp) + " ms"; 
    std::cout << out << std::endl;
    log(out);
}

void HandlerMessageClient::handler_pong(PongMessage* msg) {
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg->timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::string out = "Ping = " + std::to_string(ping_time.count()) + " ms";
    std::cout << out << "ms";
    log(out);
}

void HandlerMessageClient::handler_disconnect(DisconnectMessage* msg){
    std::string out = "Disconnected from server. Reason: " + msg->reason;
    std::cout << out << std::endl;
    log(out);
}