#include "UserServer.h"

UserSever::UserSever(std::uint16_t port, std::string ip) : Server(port, ip){
    reg_handlers();
}

void UserSever::reg_handlers(){
    handler->register_handler<PingMessage>(1, handler_ping);
    handler->register_handler<PongMessage>(2, handler_pong);
    handler->register_handler<DisconnectMessage>(3, handler_disconnect);
}