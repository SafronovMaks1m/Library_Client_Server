#pragma once
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <memory>
#include <sstream>
#include "Message.h"

class Serealize{
    friend class Connection;
    friend class HandlerMessageServer;
    friend class HandlerMessageClient;

    using MessageType = uint16_t;

    static std::string serealizer(const BaseMessage& msg);

    template <typename T>
    static void register_deserializer(MessageType id) {
        deserealizers[id] = [](const std::string& data) -> std::unique_ptr<BaseMessage> {
            return deserealizer<T>(data);
        };
    }

    template <typename T>
    static std::unique_ptr<T> deserealizer(const std::string& data){
        std::istringstream iss(data);
        boost::archive::binary_iarchive ia(iss);
        auto msg = std::make_unique<T>();
        ia >> *msg;
        return msg;
    }
    
    static std::unordered_map<MessageType, std::function<std::unique_ptr<BaseMessage>(const std::string&)>> deserealizers;
};

