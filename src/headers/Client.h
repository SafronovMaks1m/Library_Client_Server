#pragma once
#include <memory>
#include <queue>
#include "Message.h"
#include "Connection.h"

class HandlerMessageClient;

class Client{
    private:
        std::shared_ptr<Connection> _connection;
        std::queue<std::unique_ptr<BaseMessage>, std::vector<std::unique_ptr<BaseMessage>>> _messages;

        std::unique_ptr<HandlerMessageClient> handle;
        friend class HandlerMessageClient;
    public:

        void connection_request();

        void send_message(const BaseMessage& msg);

        void recv_message();

        void disconnect();
};