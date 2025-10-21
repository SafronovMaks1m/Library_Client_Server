#pragma once
#include <string>
#include "Server.h"
#include "Messages.h"

inline std::string _lastLog; //для тестов
inline void log(const std::string& message) { _lastLog = message; } // для тестов
inline const std::string& getLastLog() { return _lastLog; } // для тестов

void handler_ping(Server& server, PingMessage& msg, Connection& connection);

void handler_pong(Server& server, PongMessage& msg, Connection& connection);

void handler_disconnect(Server& server, DisconnectMessage& msg, Connection& connection);

void reg(Server& server);