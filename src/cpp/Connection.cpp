#include "Connection.h"

Connection::Connection(SOCKET socket) : _socket(socket), _running(false){};

void Connection::start_recv(){
    _running = true;
    recv_thread = std::thread(&Connection::recv, this);
}

void Connection::recv(){ 
    while(_running){} 
}

void Connection::disconnect(){
    _running = false;
    close(_socket);
    if (recv_thread.joinable())
        recv_thread.join();
}

void Connection::send(std::unique_ptr<BaseMessage>&& msg) {}