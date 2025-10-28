#include "Server.h"
#include "Connection.h"
#include "HandlerMessageServer.h"

Server::Server(uint16_t port, std::string ip): _port(port), _ip(ip), _running(false){
    handler = std::make_unique<HandlerMessageServer>(*this);
}

void Server::send_message(const BaseMessage& msg, Connection& connection) {

}

const std::vector<std::shared_ptr<Connection>>& Server::getConnections() const {
    return _connections;
}

std::vector<std::shared_ptr<Connection>>& Server::setConnections() {
    return _connections;
}

void Server::start(){
    if (_running)
        return;
    _running = true;
    main_thread = std::thread(&Server::accepting_connections, this);
}

void Server::stop(){
    if (!_running)
        return;
    _running = false;
    close(main_socket);
    for (auto it = _connections.begin(); it!=_connections.end(); it++){
        (*it)->disconnect();
    }
    if (main_thread.joinable())
        main_thread.join();
    std::cout << "Server stopped." << std::endl;
}

void Server::accepting_connections(){
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    socklen_t len_addr = sizeof(addr);

    if (inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        _running = false;
        return;
    }

    this->main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket < 0){
        std::cerr << "Failed to create socket" << std::endl;
        _running = false;
        return;
    }

    if (bind(main_socket, (sockaddr*)&addr, sizeof(addr)) < 0){
        std::cerr << "Bind failed" << std::endl;
        _running = false;
        close(main_socket);
        return;
    }

    listen(main_socket, SOMAXCONN);
    std::cout << "Server listening on " << _ip << ":" << _port << std::endl;

    SOCKET new_connection;
    while(_running){
        new_connection = accept(main_socket, (sockaddr*)&addr, &len_addr);
        if (new_connection < 0){
            if (!_running)
                break;
            std::cerr << "Accept connection failed!" << std::endl;
        }
        else{
            auto conn = std::make_shared<Connection>(new_connection);
            conn->start_recv();
            _connections.push_back(conn);

            std::cout << "New client connected!" << std::endl;
        }
    }
}

Server::~Server(){
    stop();
}
