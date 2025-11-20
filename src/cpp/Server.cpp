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
    if (_running)
        return;
    _running = true;
    recv_msg_thread = std::thread(&Server::recv_message, this);
    main_thread = std::thread(&Server::accepting_connections, this);
}

void Server::stop(){
    std::lock_guard<std::mutex> lock(_stop_mutex);
    if (!_running)
        return;

    _running = false;
    _messages_cv.notify_one();

    boost::system::error_code ec;
    _acceptor.close(ec);
    if (ec) {
        std::cerr << "Error closing acceptor: " << ec.message() << std::endl;
    }

    _service.stop();
    if (recv_msg_thread.joinable())
        recv_msg_thread.join();
    if (main_thread.joinable())
        main_thread.join();
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);
        for (auto& conn : _connections){
            conn->disconnect();
            conn.reset();
        }
        _connections.clear();
    }

    std::cout << "Server stopped." << std::endl;
}

const bool Server::is_running() const{
    return _running;
}

bool Server::has_any_messages() const{
    for (const auto& conn : _connections) {
        if (conn->has_messages()) {
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

    while (_running) {
        try {
            tcp::socket socket(_service);
            _acceptor.accept(socket, ec);
            if (ec == boost::asio::error::would_block) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            if (ec) {
                if (!_running) 
                    break;
                std::cerr << "accept error: " << ec.message() << std::endl;
                continue;
            }

            std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;
            auto conn = std::make_shared<Connection>(std::move(socket), _messages_cv, _messages_mutex);

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
    connection.send(std::move(msg));
}

void Server::recv_message() {
    while (_running) {
        std::unique_lock<std::mutex> ul(_messages_mutex);
        _messages_cv.wait(ul, [this]() { return has_any_messages() || !_running; });
        
        if (!_running) 
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
            while (conn->has_messages()) {
                auto msg = conn->pop_message();
                if (msg) {
                    ul.unlock();
                    try {
                        handler->handler(std::move(msg), *conn);
                    } 
                    catch (const std::exception& e) {
                        std::cerr << "Server Error handling message: " << e.what() << std::endl;
                    }
                    ul.lock();
                }
            }
        }
    }
}

void Server::disconnect(Connection& connection) {
    connection.disconnect();
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);
        for (auto it = _connections.begin(); it != _connections.end(); ++it) {
            if (it->get() == &connection) {
                _connections.erase(it);
                break;
            }
        }
    }
}

Server::~Server(){
    stop();
}
