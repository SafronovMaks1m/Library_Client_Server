#pragma once
#include <string>

inline std::string _lastLog; //для тестов
inline void log(const std::string& message) { _lastLog = message; } // для тестов
inline const std::string& getLastLog() { return _lastLog; } // для тестов