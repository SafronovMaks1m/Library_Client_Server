#pragma once
#include "Client.h"
#include "HandlerMessageClient.h"
#include "client_create_handlers.h"

class UserClient : public Client{
    public:
        UserClient(std::uint16_t port, std::string ip);

        void reg_handlers();
};