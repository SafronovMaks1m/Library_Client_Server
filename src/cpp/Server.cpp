#include "Server.h"
#include "HandlerMessageServer.h"

const std::map<std::string, std::unique_ptr<Connection>>& Server::getConnections() const {
    return _connections;
}