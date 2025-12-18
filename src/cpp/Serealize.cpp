#include "Serealize.h"

std::string Serealize::serealizer(const BaseMessage& msg){
    std::ostringstream oss;
    boost::archive::binary_oarchive oa(oss);
    oa << &msg;
    return oss.str();
}

std::unique_ptr<BaseMessage> Serealize::deserealizer(const std::string& data){
    std::istringstream iss(data);
    boost::archive::binary_iarchive ia(iss);
    BaseMessage* ptr = nullptr;
    ia >> ptr;
    return std::unique_ptr<BaseMessage>(ptr);
}