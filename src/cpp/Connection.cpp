#include "Connection.h"

Connection::Connection(tcp::socket&& socket, std::condition_variable& parent_cv_recv, std::mutex& parent_mutex_recv, std::condition_variable& parent_cv_send, std::mutex& parent_mutex_send) :
                                                                 _socket(std::move(socket)), _running(false),
                                                                 _parent_cv_recv(parent_cv_recv),
                                                                 _parent_mutex_recv(parent_mutex_recv),
                                                                 _parent_cv_send(parent_cv_send),
                                                                 _parent_mutex_send(parent_mutex_send){};

std::unique_ptr<BaseMessage> Connection::pop_message_recv() {
    if (_messages_recv.empty()) {
        return nullptr;
    }
    auto msg = std::move(_messages_recv.front());
    _messages_recv.pop();
    return msg;
}

std::unique_ptr<BaseMessage> Connection::pop_message_send() {
    if (_messages_send.empty()) {
        return nullptr;
    }
    auto msg = std::move(_messages_send.front());
    _messages_send.pop();
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
    _parent_cv_send.notify_all();
    if (ec) {
        std::cerr << "Error closing socket: " << ec.message() << std::endl;
    }
    if (recv_thread.joinable() && std::this_thread::get_id() != recv_thread.get_id())
        recv_thread.join();
    {
        std::lock_guard<std::mutex> lg(_parent_mutex_recv);
        while (!_messages_recv.empty())
            _messages_recv.pop();
    }
    {
        std::lock_guard<std::mutex> lg(_parent_mutex_send);
        while (!_messages_send.empty())
            _messages_send.pop();
    }
}

