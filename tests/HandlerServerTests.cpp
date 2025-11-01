#include <gtest/gtest.h>
#include "UserServer.h"

struct HandlerServerFixture : public testing::Test {
    UserSever server;
    HandlerServerFixture() : server(1111, "127.0.0.1") {}
    void SetUp() {
    }

    void TearDown() {
    }
};

TEST_F(HandlerServerFixture, HandlerServerMessagePing){
    Connection con;
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    server.handler->handler(std::make_unique<PingMessage>(milsec.count()), con);
    std::string out = "Send a pong via the send_message method on the passed connection, " 
        + std::to_string(milsec.count()) + " ms";
    EXPECT_EQ(getLastLog(), out);
}

TEST_F(HandlerServerFixture, HandlerServerMessagePong){
    Connection con;
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    server.handler->handler(std::make_unique<PongMessage>(milsec.count()), con);
    std::string log = getLastLog();
    EXPECT_NE(log.find("Ping ="), std::string::npos);
    EXPECT_NE(log.find("ms"), std::string::npos);
}

TEST_F(HandlerServerFixture, HandlerServerMessageDisconnect){
    auto con = std::make_shared<Connection>();
    server.setConnections().push_back(con);
    EXPECT_EQ(server.setConnections().size(), 1);
    server.handler->handler(std::make_unique<DisconnectMessage>("simple shutdown"), *con);
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
    EXPECT_EQ(server.setConnections().size(), 0);
}
