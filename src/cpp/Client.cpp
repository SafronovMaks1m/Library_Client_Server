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

void Client::send_message(const BaseMessage& msg) {
    
}