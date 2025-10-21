#include "HandlerMessageClient.h"

HandlerMessageClient::HandlerMessageClient(Client& client): _client(client){}

void HandlerMessageClient::handler(std::unique_ptr<BaseMessage>&& msg) {
    auto it = handlers.find(msg->type_name());
    if (it != handlers.end()) {
        it->second(*msg);
    }
    else {
        std::cout << "no handler for the message";
    }
}