#pragma once
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "Message.h"
#include <iostream>

class Serealize{
    friend class Connection;
    friend class HandlerMessageServer;
    friend class HandlerMessageClient;

    using MessageType = uint16_t;

    static std::string serealizer(const BaseMessage& msg);

    static std::unique_ptr<BaseMessage> deserealizer(const std::string& data);
};