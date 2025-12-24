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
        std::atomic<bool> _running;
        mutable std::mutex _connection_mutex;
        std::thread recv_msg_thread;
        std::thread send_msg_thread;
        std::mutex _messages_mutex_send;
        std::condition_variable _messages_cv_send;

        ThreadSafeQueue<std::shared_ptr<Connection>> _recv_notify_queue;

        std::atomic<bool> _reconnecting{false};
        int _connection_attempts;
        io_service _service;
        std::shared_ptr<Connection> _connection;

        void recv_message();

        void send_message_thd();

        friend class Connection;
        friend class HandlerMessageClient;
    public:

        Client(std::uint16_t port, std::string ip);

        void reconnecting();

        void start();

        void stop();

        const bool is_running() const;

        const std::shared_ptr<Connection>& getConnection() const;

        std::unique_ptr<HandlerMessageClient> handler;
        
        void connection_request();

        void send_message(std::unique_ptr<BaseMessage>&& msg);

        ~Client();
};