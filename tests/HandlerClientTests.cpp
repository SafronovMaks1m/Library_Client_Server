#include <gtest/gtest.h>
#include "UserClient.h"

struct HandlerClientFixture : public testing::Test {
    UserClient client;
    HandlerClientFixture() : client(1111, "127.0.0.1") {}
    void SetUp() {
    }

    void TearDown() {
    }
};

TEST_F(HandlerClientFixture, HandlerClientMessageConnectionAccepted){
    client.handler->handler(std::make_unique<ConnectionAcceptMessage>(true));
    EXPECT_EQ(getLastLog(), "Connection successful");
}

TEST_F(HandlerClientFixture, HandlerClientMessageConnectionNotAccepted){
    auto msg = std::make_unique<ConnectionAcceptMessage>(false, "Just like that");
    client.handler->handler(std::move(msg));
    EXPECT_EQ(getLastLog(), "Connection failed. Error: Just like that");
}

TEST_F(HandlerClientFixture, HandlerClientMessagePing){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    client.handler->handler(std::make_unique<PingMessage>(milsec.count()));
    std::string out = "send pong to the server with " 
        + std::to_string(milsec.count()) + " ms";
    EXPECT_EQ(getLastLog(), out);
}

TEST_F(HandlerClientFixture, HandlerClientMessagePong){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    client.handler->handler(std::make_unique<PongMessage>(milsec.count()));
    std::string log = getLastLog();
    EXPECT_NE(log.find("Ping ="), std::string::npos);
    EXPECT_NE(log.find("ms"), std::string::npos);
}

TEST_F(HandlerClientFixture, HandlerClientMessageDisconnect){
    client.handler->handler(std::make_unique<DisconnectMessage>("simple shutdown"));
    std::string out = "Disconnected from server. Reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
}