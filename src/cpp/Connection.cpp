#include "Connection.h"

Connection::Connection(tcp::socket&& socket) : _socket(std::move(socket)), _running(false){};

void Connection::disconnect(){
    _running = false;
    boost::system::error_code ec;
    if (_socket.is_open())
        _socket.close(ec);
    if (ec) {
        std::cerr << "Error closing socket: " << ec.message() << std::endl;
    }
    if (recv_thread.joinable())
        recv_thread.join();
}
