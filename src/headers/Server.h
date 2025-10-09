#pragma once
#include <map>
#include <memory>
#include <queue>
#include "Message.h"
#include <string>
#include "Connection.h"
#include "CompareMessage.h"
#include "HandlerMessage.h"

class HandlerMessageServer;

class Server {
    private:
        std::map<std::string, std::unique_ptr<Connection>> _connections;
        std::priority_queue<std::unique_ptr<BaseMessage>, std::vector<std::unique_ptr<BaseMessage>>, CompareMessage> _messages;
        std::unique_ptr<HandlerMessage> handle;
        friend class HandlerMessageServer;
        
    public:
        const std::map<std::string, std::unique_ptr<Connection>>& getConnections() const;
};