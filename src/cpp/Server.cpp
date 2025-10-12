#include "Server.h"
#include "Connection.h"
#include "HandlerMessageServer.h"

const std::vector<std::shared_ptr<Connection>>& Server::getConnections() const {
    return _connections;
}