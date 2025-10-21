#include <algorithm>
#include "HandlerMessageServer.h"

HandlerMessageServer::HandlerMessageServer(Server& server): _server(server){}

void HandlerMessageServer::handler(std::unique_ptr<BaseMessage>&& msg, Connection& connection) {
    std::cout << "Handler got message type: " << msg->type_name() << std::endl;
    auto it = handlers.find(msg->type_name());
    if (it != handlers.end()) {
        std::cout << "Handler found!" << std::endl;
        it->second(*msg, connection);
        
    }
    else {
        std::cout << "no handler for the message";
    }
}