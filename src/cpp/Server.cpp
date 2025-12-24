#include "Server.h"
#include "Connection.h"
#include "HandlerMessageServer.h"

Server::Server(uint16_t port, std::string ip): _port(port), _ip(ip),
                                               _running(false), _service(), 
                                               _acceptor(_service, tcp::endpoint(ip::address::from_string(ip), port)){
    handler = std::make_unique<HandlerMessageServer>(*this);
}

const std::vector<std::shared_ptr<Connection>>& Server::getConnections() const {
    std::lock_guard<std::mutex> lock(_connection_mutex);
    return _connections;
}

void Server::start(){
    if (_running.load())
        return;
    _running.store(true);
    recv_msg_thread = std::thread(&Server::recv_message, this);
    send_msg_thread = std::thread(&Server::send_message_thd, this);
    main_thread = std::thread(&Server::accepting_connections, this);
}

void Server::stop(){
    if (!_running.load())
        return;

    _running.store(false);
    _recv_notify_queue.push(std::shared_ptr<Connection>{});
    _messages_cv_send.notify_one();
    boost::system::error_code ec;
    _acceptor.close(ec);
    if (ec) {
        std::cerr << "Error closing acceptor: " << ec.message() << std::endl;
    }
    _service.stop();
    if (recv_msg_thread.joinable())
        recv_msg_thread.join();
    if (send_msg_thread.joinable())
        send_msg_thread.join();
    if (main_thread.joinable())
        main_thread.join();
    std::vector<std::shared_ptr<Connection>> copy_conns;
    {
        std::lock_guard<std::mutex> conn_lock(_connection_mutex);
        copy_conns = std::move(_connections);
        _connections.clear();
    }
    for (auto &c : copy_conns) {
        if (c) c->disconnect();
    }
    _service.reset();
    std::cout << "Server stopped." << std::endl;
}

const bool Server::is_running() const{
    return _running.load();
}

bool Server::has_any_messages_recv() const{
    std::lock_guard<std::mutex> lock(_connection_mutex);
    for (const auto& conn : _connections) {
        if (conn->has_messages_recv()) {
            return true;
        }
    }
    return false;
}

bool Server::has_any_messages_send() const{
    std::lock_guard<std::mutex> lock(_connection_mutex);
    for (const auto& conn : _connections) {
        if (conn->has_messages_send()) {
            return true;
        }
    }
    return false;
}

void Server::accepting_connections(){
    boost::system::error_code ec;
    _acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cerr << "Listen failed: " << ec.message() << std::endl;
        std::thread([this]{ stop(); }).detach();
        return;
    }

    _acceptor.non_blocking(true, ec);
    if (ec) {
        std::cerr << "Failed to set non-blocking mode: " << ec.message() << std::endl;
        std::thread([this]{ stop(); }).detach();
        return;
    }

    while (_running.load()) {
        try {
            tcp::socket socket(_service);
            _acceptor.accept(socket, ec);
            if (ec == boost::asio::error::would_block) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            if (ec) {
                if (!_running.load()) 
                    break;
                std::cerr << "accept error: " << ec.message() << std::endl;
                continue;
            }

            std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;
            auto conn = std::make_shared<Connection>(std::move(socket), _recv_notify_queue);

            conn->on_disconnect = [this, conn_ptr = conn.get()]() { 
                this->disconnect(*conn_ptr); 
            };
            conn->on_error = [this]() { stop(); };

            {
                std::lock_guard<std::mutex> lock(_connection_mutex);
                _connections.push_back(conn);
            }
            conn->start_recv();
        } 
        catch (std::exception& e) {
            std::cerr << "Error accepting connection: " << e.what() << std::endl;
        }
    }
}

void Server::send_message(std::unique_ptr<BaseMessage>&& msg, Connection& connection) {
    if (!_running.load() || !connection.is_running()) return;

    connection._messages_send.push_wait(std::move(msg), 10);
    
    _messages_cv_send.notify_one();
}

void Server::send_message_thd(){
    while (_running.load()) {
        std::unique_lock<std::mutex> ul(_messages_mutex_send);
        _messages_cv_send.wait(ul, [this]() { return has_any_messages_send() || !_running.load(); });
        
        if (!_running.load()) 
            break;

        std::vector<std::shared_ptr<Connection>> connections_copy;
        {
            std::lock_guard<std::mutex> conn_lock(_connection_mutex);
            connections_copy = _connections;
        }

        for (auto& conn : connections_copy) {
            if (!conn->is_running()) {
                continue;
            }
            while (conn->has_messages_send()) {
                if (!conn->is_running()) {
                    break;
                }
                auto msg = conn->pop_message_send();
                if (msg) {
                    ul.unlock();
                    conn->send(std::move(msg));
                    ul.lock();
                }
            }
        }
    }
}

void Server::recv_message() {
    while (_running.load()) {
        std::shared_ptr<Connection> conn;
        try {
            conn = _recv_notify_queue.wait_and_pop();
        } catch (...) {
            continue;
        }
        if (!conn) 
            continue;

        while (conn->is_running() && conn->has_messages_recv()) {
            auto msg = conn->pop_message_recv();
            if (!msg) 
                break;
            try {
                handler->handler(std::move(msg), *conn);
            } catch (const std::exception& e) {
                std::cerr << "Server Error handling message: " << e.what() << std::endl;
            }
        }
    }
}

void Server::disconnect(Connection& connection) {
    std::shared_ptr<Connection> copy_conn;
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);
        for (auto it = _connections.begin(); it != _connections.end(); ++it) {
            if (it->get() == &connection) {
                copy_conn = *it;
                _connections.erase(it);
                break;
            }
        }
    }
    if (copy_conn) {
        copy_conn->disconnect();
    }
}

Server::~Server(){
    stop();
}
