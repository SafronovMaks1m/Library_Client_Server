#include "Connection.h"

Connection::Connection(tcp::socket&& socket, std::condition_variable& parent_cv, std::mutex& parent_mutex) :
                                                                 _socket(std::move(socket)), _running(false),
                                                                 _parent_cv(parent_cv),
                                                                 _parent_mutex(parent_mutex){};

std::unique_ptr<BaseMessage> Connection::pop_message() {
    if (_messages.empty()) {
        return nullptr;
    }
    auto msg = std::move(_messages.front());
    _messages.pop();
    return msg;
}

bool Connection::is_running(){
    return _running.load();
}

void Connection::disconnect(){
    _running.store(false); 
    boost::system::error_code ec;
    if (_socket.is_open()){
        _socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
        _socket.close(ec);
    }
    if (ec) {
        std::cerr << "Error closing socket: " << ec.message() << std::endl;
    }
    if (recv_thread.joinable() && std::this_thread::get_id() != recv_thread.get_id())
        recv_thread.join();
    std::lock_guard<std::mutex> lg(_parent_mutex);
    while (!_messages.empty())
        _messages.pop();
}

