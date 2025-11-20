#include <gtest/gtest.h>
#include "UserClient.h"
#include "UserServer.h"
#include "thread"
#include "iostream"

struct ConnectionFixture : public testing::Test {
    UserSever server;
    ConnectionFixture() : server(49152, "127.0.0.1") {}
    void SetUp() {
    }

    void TearDown() { 
    }
};

TEST_F(ConnectionFixture, StartAndStop_Server){
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.is_running(), true);
    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.is_running(), false);
}

TEST_F(ConnectionFixture, StartAndStopClientConnectedToServer){
    UserClient client(49152, "127.0.0.1");
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(client.is_running(), true);
    client.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(client.is_running(), false);
}

TEST_F(ConnectionFixture, StartAndStopClientNotConnectedToServer){
    UserClient client(49153, "127.0.0.1");
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(client.is_running(), false);
    client.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(client.is_running(), false);
}

TEST_F(ConnectionFixture, СonnectingСlienToServerAndReconnectionCheck){
    UserClient client(49152, "127.0.0.1");
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(12000));
}

TEST_F(ConnectionFixture, СonnectingСlienToServerAndDisconnectCheckServer){
    UserClient client(49152, "127.0.0.1");
    server.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(12000));
}