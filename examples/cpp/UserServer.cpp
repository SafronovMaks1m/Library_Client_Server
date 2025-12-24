#include "UserServer.h"

UserSever::UserSever(std::uint16_t port, std::string ip) : Server(port, ip){
    reg_handlers();
}

void UserSever::reg_handlers(){
    handler->register_handler<PingMessage>(handler_ping);
    handler->register_handler<PongMessage>(handler_pong);
    handler->register_handler<DisconnectMessage>(handler_disconnect);
    handler->register_handler<SimpleMessage>(handler_simple);
    handler->register_handler<ForwardingMessage>(handler_forwarding);
}