#pragma once
#include <memory>
#include <queue>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Message.h"
#include "Connection.h"

class HandlerMessageClient;

class Client{
    private:
        std::uint16_t _port;
        std::string _ip;
        bool _running;

        std::shared_ptr<Connection> _connection;
        std::queue<std::unique_ptr<BaseMessage>, std::vector<std::unique_ptr<BaseMessage>>> _messages;

        friend class HandlerMessageClient;
    public:
        using SOCKET = int;

        Client(std::uint16_t port, std::string ip);

        void start();

        void stop();

        const std::shared_ptr<Connection>& getConnection() const;

        std::shared_ptr<Connection>& setConnection();

        std::unique_ptr<HandlerMessageClient> handler;
        
        void connection_request();

        void send_message(const BaseMessage& msg);

        void recv_message();

        void disconnect();

        ~Client();
};