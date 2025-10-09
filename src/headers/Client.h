#include <memory>
#include <queue>
#include "Message.h"
#include "Connection.h"
#include "CompareMessage.h"
#include "HandlerMessage.h"

class HandlerMessageClient;

class Client{
    private:
        std::shared_ptr<Connection> _server;
        std::priority_queue<std::unique_ptr<BaseMessage>, std::vector<std::unique_ptr<BaseMessage>>, CompareMessage> _messages;

        std::unique_ptr<HandlerMessage> handle;
        friend class HandlerMessageClient;
};