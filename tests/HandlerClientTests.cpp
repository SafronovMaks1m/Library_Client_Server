#include <gtest/gtest.h>
#include "Client.h"
#include "HandlerMessageClient.h"

struct HandlerClientFixture : public testing::Test {
    Client server;
    HandlerMessageClient* handler;

    void SetUp() {
        handler = new HandlerMessageClient(server);
    }

    void TearDown() {
        delete handler;
    }
};

TEST_F(HandlerClientFixture, HandlerClientMessageConnectionAccepted){
    handler->handler(std::make_unique<ConnectionAcceptMessage>(true));
    EXPECT_EQ(handler->getLastLog(), "Connection successful");
}

TEST_F(HandlerClientFixture, HandlerClientMessageConnectionNotAccepted){
    auto msg = std::make_unique<ConnectionAcceptMessage>(false, "Just like that");
    handler->handler(std::move(msg));
    EXPECT_EQ(handler->getLastLog(), "Connection failed. Error: Just like that");
}

TEST_F(HandlerClientFixture, HandlerClientMessagePing){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    handler->handler(std::make_unique<PingMessage>(milsec.count()));
    std::string out = "send pong to the server with " 
        + std::to_string(milsec.count()) + " ms";
    EXPECT_EQ(handler->getLastLog(), out);
}

TEST_F(HandlerClientFixture, HandlerClientMessagePong){
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    handler->handler(std::make_unique<PongMessage>(milsec.count()));
    std::string log = handler->getLastLog();
    EXPECT_NE(log.find("Ping ="), std::string::npos);
    EXPECT_NE(log.find("ms"), std::string::npos);
}

TEST_F(HandlerClientFixture, HandlerClientMessageDisconnect){
    auto con = std::make_shared<Connection>();
    handler->handler(std::make_unique<DisconnectMessage>("simple shutdown"));
    std::string out = "Disconnected from server. Reason: simple shutdown";
    std::string log = handler->getLastLog();
    EXPECT_EQ(log, out);
}

TEST_F(HandlerClientFixture, HandlerClientMessageUnknown){
    struct SomeMessage : BaseMessage {};
    handler->handler(std::make_unique<SomeMessage>());
    EXPECT_EQ(handler->getLastLog(), "no handler for the message");
}
