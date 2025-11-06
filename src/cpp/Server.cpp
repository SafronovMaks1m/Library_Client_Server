#include "Server.h"
#include "Connection.h"
#include "HandlerMessageServer.h"

Server::Server(uint16_t port, std::string ip): _port(port), _ip(ip),
                                               _running(false), _service(), 
                                               _acceptor(_service, tcp::endpoint(ip::address::from_string(ip), port)){
    handler = std::make_unique<HandlerMessageServer>(*this);
}

void Server::add_message(std::shared_ptr<Connection> con, std::unique_ptr<BaseMessage>&& msg){
    std::lock_guard<std::mutex> lock(_messages_mutex);
    _messages.emplace(con, std::move(msg));
    _messages_cv.notify_one();
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

    while (!_messages.empty())
        _messages.pop();
    std::cout << "Server stopped." << std::endl;
}


void Server::accepting_connections(){
    boost::system::error_code ec;
    _acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cerr << "Listen failed: " << ec.message() << std::endl;
        _running = false;
        return;
    }
    while (_running) {
        try {
            tcp::socket socket(_service);
            _acceptor.accept(socket); 
            if (!_running) {
                socket.close();
                break;
            }
            std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;
            auto conn = std::make_shared<Connection>(std::move(socket));
            {
                std::lock_guard<std::mutex> lock(_connection_mutex);
                _connections.push_back(conn);
            }
            conn->start_recv(*this);
        } 
        catch (std::exception& e) {
            std::cerr << "Error accepting connection: " << e.what() << std::endl;
        }
    }
}

void Server::send_message(std::unique_ptr<BaseMessage>&& msg, Connection& connection) {
    connection.send(std::move(msg), *this);
}

void Server::recv_message() {
    std::unique_lock<std::mutex> ul(_messages_mutex, std::defer_lock);

    while (_running) {

        _messages_cv.wait(ul, [this]() { return !_messages.empty() || !_running; });

        if (!_running) break;

        auto conn = std::move(_messages.front().first);
        auto msg = std::move(_messages.front().second);
        _messages.pop();

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
    
    std::lock_guard<std::mutex> lock(_messages_mutex);

    std::queue<std::pair<std::shared_ptr<Connection>, std::unique_ptr<BaseMessage>>> filtered;
    while (!_messages.empty()) {
        auto& con = _messages.front().first;
        if (con.get() != &connection)
            filtered.push(std::move(_messages.front()));
        _messages.pop();
    }
    std::swap(_messages, filtered);
}

Server::~Server(){
    stop();
}
