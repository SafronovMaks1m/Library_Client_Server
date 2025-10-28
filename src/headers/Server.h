#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <thread>
#include <mutex>
#include "Connection.h"
#include "Message.h"

class HandlerMessageServer;

class Server {
    private:
        using SOCKET = int;
        uint16_t _port;
        std::string _ip;
        bool _running;
        std::thread main_thread;
        SOCKET main_socket;

        std::vector<std::shared_ptr<Connection>> _connections;
        std::queue<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>, 
            std::vector<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>>> _messages;
        
        friend class HandlerServerFixture_HandlerServerMessageDisconnect_Test;
    public:
        
        std::unique_ptr<HandlerMessageServer> handler;

        Server(uint16_t port, std::string ip);

        const std::vector<std::shared_ptr<Connection>>& getConnections() const;

        std::vector<std::shared_ptr<Connection>>& setConnections();

        void start();

        void stop();

        void accepting_connections();

        void send_message(const BaseMessage& msg, Connection& connection);

        void recv_message();

        void disconnect();

        ~Server();
};