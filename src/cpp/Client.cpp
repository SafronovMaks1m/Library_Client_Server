#include <mutex>
#include "Client.h"
#include "HandlerMessageClient.h"

Client::Client(uint16_t port, std::string ip): _port(port), _ip(ip){
    handler = std::make_unique<HandlerMessageClient>(*this);
}

const std::shared_ptr<Connection>& Client::getConnection() const{
    return _connection;
}

std::shared_ptr<Connection>& Client::setConnection(){ //для тестов
    return _connection;
}

void Client::start(){
    if (_running)
        return;
    _running = true;
    connection_request();
}

void Client::stop(){
    if (!_running)
        return;
    _running = false;
    _connection->disconnect();
    std::cout << "Client stopped." << std::endl;
}

void Client::connection_request(){
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    socklen_t len_addr = sizeof(addr); 

    if (inet_pton(AF_INET, _ip.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address" << std::endl;
        _running = false;
        return;
    }

    SOCKET conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn < 0){
        std::cerr << "Failed to create socket" << std::endl;
        _running = false;
        return;
    }

    if (connect(conn, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        _running = false;
        close(conn);
        return;
    }

    std::cout << "Connected!" << std::endl;
    _connection = std::make_shared<Connection>(conn);
    _connection->start_recv();
}

Client::~Client(){
    stop();
}

void Client::send_message(const BaseMessage& msg) {
    
}