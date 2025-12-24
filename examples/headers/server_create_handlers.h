#pragma once
#include <string>
#include "Server.h"
#include "Messages.h"
#include "logs.h" //для тестов

void handler_ping(Server& server, PingMessage& msg, Connection& connection);

void handler_pong(Server& server, PongMessage& msg, Connection& connection);

void handler_disconnect(Server& server, DisconnectMessage& msg, Connection& connection);

void handler_simple(Server& server, SimpleMessage& msg, Connection& connection);

void handler_forwarding(Server& server, ForwardingMessage& msg, Connection& connection);