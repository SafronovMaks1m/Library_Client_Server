#pragma once
#include <memory>
#include <queue>
#include <cstdint>
#include <string>
#include "Message.h"
#include "Connection.h"

class HandlerMessageClient;

class Client{
    private:
        std::uint16_t _port;
        std::string _ip;

        std::shared_ptr<Connection> _connection;
        std::queue<std::unique_ptr<BaseMessage>, std::vector<std::unique_ptr<BaseMessage>>> _messages;

        friend class HandlerMessageClient;
    public:
        Client(std::uint16_t port, std::string ip);

        const std::shared_ptr<Connection>& getConnection() const;

        std::shared_ptr<Connection>& setConnection();

        std::unique_ptr<HandlerMessageClient> handler;
        
        void connection_request();

        void send_message(const BaseMessage& msg);

        void recv_message();

        void disconnect();
};