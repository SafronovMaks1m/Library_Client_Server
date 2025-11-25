#include "Serealize.h"

std::unordered_map<Serealize::MessageType,std::function<std::unique_ptr<BaseMessage>(const std::string&)>> Serealize::deserealizers;
std::unordered_map<Serealize::MessageType,std::function<std::string(const BaseMessage&)>> Serealize::serializers;