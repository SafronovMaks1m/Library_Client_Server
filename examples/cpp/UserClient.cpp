#include "UserClient.h"

UserClient::UserClient(std::uint16_t port, std::string ip) : Client(port, ip){
    reg_handlers();
}

void UserClient::reg_handlers(){
    handler->register_handler<ConnectionAcceptMessage>(4, handler_connect);
    handler->register_handler<PingMessage>(1, handler_ping);
    handler->register_handler<PongMessage>(2, handler_pong);
    handler->register_handler<DisconnectMessage>(3, handler_disconnect);
}