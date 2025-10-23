#pragma once
#include <string>
#include "Server.h"
#include "Messages.h"
#include "logs.h" //для тестов

void handler_ping(Server& server, PingMessage& msg, Connection& connection);

void handler_pong(Server& server, PongMessage& msg, Connection& connection);

void handler_disconnect(Server& server, DisconnectMessage& msg, Connection& connection);