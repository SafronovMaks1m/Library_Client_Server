#pragma once
#include "Server.h"
#include "HandlerMessageServer.h"
#include "server_create_handlers.h"

class UserSever : public Server{
    public:
        UserSever(std::uint16_t port, std::string ip);

        void reg_handlers();
};