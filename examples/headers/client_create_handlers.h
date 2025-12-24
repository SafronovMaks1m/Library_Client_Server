#pragma once
#include <string>
#include "Client.h"
#include "Messages.h"
#include "logs.h"//для тестов

void handler_connect(Client& client, ConnectionAcceptMessage& msg);

void handler_ping(Client& client, PingMessage& msg);

void handler_pong(Client& client, PongMessage& msg);

void handler_disconnect(Client& client, DisconnectMessage& msg);

void handler_simple(Client& client, SimpleMessage& msg);