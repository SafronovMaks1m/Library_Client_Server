#include "HandlerMessageClient.h"

HandlerMessageClient::HandlerMessageClient(Client& client): _client(client){}

void HandlerMessageClient::handler(std::unique_ptr<BaseMessage>&& msg) {
    MessageType id = msg->getType();
    auto it = handlers.find(id);
    if (it != handlers.end()) {
        it->second(*msg);
    }
    else {
        std::cout << "No handler for the message" << std::endl;
    }
}