#include <gtest/gtest.h>
#include "Server.h"
#include "HandlerMessageServer.h"

struct HandlerServerFixture : public testing::Test {
    Server server;
    HandlerMessageServer* handler;

    void SetUp() {
        handler = new HandlerMessageServer(server);
    }

    void TearDown() {
        delete handler;
    }
};

TEST_F(HandlerServerFixture, HandlerServerMessagePing){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    handler->handler(std::make_unique<PingMessage>(milsec.count()), std::make_unique<Connection>());
    std::string out = "Send a pong via the send_message method on the passed connection, " 
        + std::to_string(milsec.count()) + " ms";
    EXPECT_EQ(handler->getLastLog(), out);
}

TEST_F(HandlerServerFixture, HandlerServerMessagePong){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    handler->handler(std::make_unique<PongMessage>(milsec.count()), std::make_unique<Connection>());
    std::string log = handler->getLastLog();
    EXPECT_NE(log.find("Ping ="), std::string::npos);
    EXPECT_NE(log.find("ms"), std::string::npos);
}

TEST_F(HandlerServerFixture, HandlerServerMessageDisconnect){
    auto con = std::make_shared<Connection>();
    server._connections.push_back(con);
    EXPECT_EQ(server._connections.size(), 1);
    handler->handler(std::make_unique<DisconnectMessage>("simple shutdown"), con);
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = handler->getLastLog();
    EXPECT_EQ(log, out);
    EXPECT_EQ(server._connections.size(), 0);
}

TEST_F(HandlerServerFixture, HandlerServerMessageUnknown){
    struct SomeMessage : BaseMessage {};
    handler->handler(std::make_unique<SomeMessage>(), std::make_shared<Connection>());
    EXPECT_EQ(handler->getLastLog(), "no handler for the message"); 
}
