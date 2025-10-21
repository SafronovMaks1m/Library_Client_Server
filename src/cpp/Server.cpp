#include "Server.h"
#include "Connection.h"
#include "HandlerMessageServer.h"

Server::Server(uint16_t port, std::string ip): _port(port), _ip(ip){
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