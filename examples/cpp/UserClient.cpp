#include "UserClient.h"

UserClient::UserClient(std::uint16_t port, std::string ip) : Client(port, ip){
    reg_handlers();
}

void UserClient::reg_handlers(){
    handler->register_handler<ConnectionAcceptMessage>(handler_connect);
    handler->register_handler<PingMessage>(handler_ping);
    handler->register_handler<PongMessage>(handler_pong);
    handler->register_handler<DisconnectMessage>(handler_disconnect);
    handler->register_handler<SimpleMessage>(handler_simple);
}