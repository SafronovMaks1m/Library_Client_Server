#pragma once
#include <string>
#include "Client.h"
#include "Messages.h"

inline std::string _lastLog; //для тестов
inline void log(const std::string& message) { _lastLog = message; } //для тестов
inline const std::string& getLastLog() { return _lastLog; } // для тестов

void handler_connect(Client& client, ConnectionAcceptMessage& msg);

void handler_ping(Client& client, PingMessage& msg);

void handler_pong(Client& client, PongMessage& msg);

void handler_disconnect(Client& client,DisconnectMessage& msg);

void reg(Client& client);