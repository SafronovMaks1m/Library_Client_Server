#pragma once
#include <memory>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "Message.h"

class Connection {
    private:
        using SOCKET = int;
        SOCKET _socket;
        std::thread recv_thread;
        bool _running;
    public:
        Connection() = default;
        explicit Connection(SOCKET socket);
        void send(std::unique_ptr<BaseMessage>&& msg);
        void start_recv();
        void recv();
        void disconnect();
};