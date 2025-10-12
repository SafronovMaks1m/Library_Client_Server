#include <algorithm>
#include "HandlerMessageServer.h"

HandlerMessageServer::HandlerMessageServer(Server& server): _server(server){}

void HandlerMessageServer::handler(std::unique_ptr<BaseMessage>&& msg, const std::shared_ptr<Connection>& connection) {
    if (auto* mes = dynamic_cast<PingMessage*>(msg.get())) {
        handler_ping(mes, connection);
    }
    else if (auto* mes = dynamic_cast<PongMessage*>(msg.get())) {
        handler_pong(mes);
    }
    else if (auto* mes = dynamic_cast<DisconnectMessage*>(msg.get())){
        handler_disconnect(mes, connection);
    }
    else {
        std::cout << "no handler for the message";
        log("no handler for the message");
    }
}

void HandlerMessageServer::handler_ping(PingMessage* msg, const std::shared_ptr<Connection>& connection) {
    connection->send(std::make_unique<PongMessage>(msg->timestamp));
    std::string out = "Send a pong via the send_message method on the passed connection, " 
        + std::to_string(msg->timestamp) + " ms";
    std::cout << out;
    log(out);
}

void HandlerMessageServer::handler_pong(PongMessage* msg) {
    auto now = std::chrono::system_clock::now();
    auto initial_time = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(msg->timestamp)
    );
    
    auto ping_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - initial_time);
    std::string out = "Ping = " + std::to_string(ping_time.count()) + " ms";
    std::cout << out << std::endl;
    log(out);
}

void HandlerMessageServer::handler_disconnect(DisconnectMessage* msg, const std::shared_ptr<Connection>& connection){
    std::string out = "Client disconnected, reason: " + msg->reason;
    for (auto it = _server._connections.begin(); it != _server._connections.end(); it++) {
        if (it->get() == connection.get()) {
            _server._connections.erase(it);
            break;                         
        }
    }
    std::cout << "Client disconnected, reason: " << msg->reason << std::endl;
    log(out);
}