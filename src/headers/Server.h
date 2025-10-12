#pragma once
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include "Connection.h"
#include "Message.h"

class HandlerMessageServer;

class Server {
    private:
        std::vector<std::shared_ptr<Connection>> _connections;
        std::queue<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>, 
            std::vector<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>>> _messages;
        std::unique_ptr<HandlerMessageServer> handle;
        
        friend class HandlerMessageServer;
        friend class HandlerServerFixture_HandlerServerMessageDisconnect_Test;
    public:
        const std::vector<std::shared_ptr<Connection>>& getConnections() const;

        void accepting_connections();

        void send_message(const BaseMessage& msg);

        void recv_message();

        void disconnect();
};