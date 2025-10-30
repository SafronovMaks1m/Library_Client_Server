#include <algorithm>
#include "HandlerMessageServer.h"

HandlerMessageServer::HandlerMessageServer(Server& server): _server(server){}

void HandlerMessageServer::handler(std::unique_ptr<BaseMessage>&& msg, Connection& connection) {
    MessageType id = msg->getType();
    auto it = handlers.find(id);
    if (it != handlers.end()) {
        std::cout << "Handler found!" << std::endl;
        it->second(*msg, connection);
    }
    else {
        std::cout << "No handler for the message" << std::endl;
    }
}