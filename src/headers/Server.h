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
        uint16_t _port;
        std::string _ip;

        std::vector<std::shared_ptr<Connection>> _connections;
        std::queue<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>, 
            std::vector<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>>> _messages;
        
        friend class HandlerServerFixture_HandlerServerMessageDisconnect_Test;
    public:
        std::unique_ptr<HandlerMessageServer> handler;

        Server(uint16_t port, std::string ip);

        const std::vector<std::shared_ptr<Connection>>& getConnections() const;

        std::vector<std::shared_ptr<Connection>>& setConnections();

        void accepting_connections();

        void send_message(const BaseMessage& msg, Connection& connection);

        void recv_message();

        void disconnect();
};