#include "Serealize.h"

std::unordered_map<Serealize::MessageType,std::function<std::unique_ptr<BaseMessage>(const std::string&)>> Serealize::deserealizers;

std::string Serealize::serealizer(const BaseMessage& msg){
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << msg;
    return oss.str();
}
