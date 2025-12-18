#pragma once
#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "Connection.h"
#include "Message.h"
#include "ThreadSafeQueue.h"

class HandlerMessageServer;

using namespace boost::asio;
using boost::asio::ip::tcp;

class Server {
    private:
        uint16_t _port;
        std::string _ip;
        std::atomic<bool> _running;

        std::thread main_thread;
        std::thread recv_msg_thread;
        std::thread send_msg_thread;
        std::mutex _messages_mutex_send;
        std::condition_variable _messages_cv_send;
        mutable std::mutex _connection_mutex;

        io_service _service;
        tcp::acceptor _acceptor;
        std::vector<std::shared_ptr<Connection>> _connections;

        ThreadSafeQueue<std::shared_ptr<Connection>> _recv_notify_queue;

        void recv_message();

        void send_message_thd();

        friend class HandlerServerFixture_HandlerServerMessageDisconnect_Test;
        friend class Connection;
    public:
        
        std::unique_ptr<HandlerMessageServer> handler;

        Server(uint16_t port, std::string ip);

        const std::vector<std::shared_ptr<Connection>>& getConnections() const;

        const bool is_running() const;

        void start();

        void stop();

        void accepting_connections();

        void send_message(std::unique_ptr<BaseMessage>&& msg, Connection& connection);

        void disconnect(Connection& connection);
        
        bool has_any_messages_recv() const;

        bool has_any_messages_send() const;

        ~Server();
};