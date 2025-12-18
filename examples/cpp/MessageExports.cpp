#include "Messages.h"
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <iostream>

BOOST_CLASS_EXPORT(PingMessage)
BOOST_CLASS_EXPORT(PongMessage)
BOOST_CLASS_EXPORT(DisconnectMessage)
BOOST_CLASS_EXPORT(ConnectionAcceptMessage)
BOOST_CLASS_EXPORT(SimpleMessage)
BOOST_CLASS_EXPORT(ForwardingMessage)