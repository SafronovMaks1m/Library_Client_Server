#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <cstdint>
#include <string>
#include <condition_variable>
#include <atomic>
#include "Message.h"
#include "Connection.h"

class HandlerMessageClient;

using namespace boost::asio;

class Client{
    private:
        std::uint16_t _port;
        std::string _ip;
        bool _running;
        std::mutex _messages_mutex;
        mutable std::mutex _connection_mutex;
        std::condition_variable _messages_cv;
        std::thread recv_msg_thread;
        std::mutex _stop_mutex;

        std::atomic<bool> _reconnecting{false};
        int _connection_attempts;
        io_service _service;
        std::shared_ptr<Connection> _connection;
        std::queue<std::unique_ptr<BaseMessage>> _messages;

        void add_message(std::shared_ptr<Connection> con, std::unique_ptr<BaseMessage>&& msg);

        friend class Connection;
        friend class HandlerMessageClient;
    public:

        Client(std::uint16_t port, std::string ip);

        void reconnecting();

        void start();

        void stop();

        const std::shared_ptr<Connection>& getConnection() const;

        std::unique_ptr<HandlerMessageClient> handler;
        
        void connection_request();

        void send_message(std::unique_ptr<BaseMessage>&& msg);

        void recv_message();

        ~Client();
};