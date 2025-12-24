#include "Connection.h"
#include "ThreadSafeQueue.h"
#include <memory>

Connection::Connection(tcp::socket&& socket, ThreadSafeQueue<std::shared_ptr<Connection>>& own_notify): _socket(std::move(socket)),
                                                                                            _own_notify(own_notify),
                                                                                            _running(false){}

std::unique_ptr<BaseMessage> Connection::pop_message_recv() {
    if (_messages_recv.empty()) {
        return nullptr;
    }
    auto msg = std::move(_messages_recv.pop_front());
    return msg;
}

std::unique_ptr<BaseMessage> Connection::pop_message_send() {
    if (_messages_send.empty()) {
        return nullptr;
    }
    auto msg = std::move(_messages_send.pop_front());
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
    _messages_recv.clear();
    _messages_send.clear();
}

