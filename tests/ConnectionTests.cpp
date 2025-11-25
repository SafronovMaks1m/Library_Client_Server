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

TEST_F(ConnectionFixture, StartAndStopServer){
    server.start();
    EXPECT_EQ(server.is_running(), true);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    server.stop();
    EXPECT_EQ(server.is_running(), false);
}

TEST_F(ConnectionFixture, StartAndStopClientConnectedToServer){
    UserClient client(49152, "127.0.0.1");
    client.start();
    EXPECT_EQ(client.is_running(), true);
    client.stop();
    EXPECT_EQ(client.is_running(), false);
}

TEST_F(ConnectionFixture, StartAndStopClientNotConnectedToServer){
    UserClient client(49153, "127.0.0.1");
    client.start();
    EXPECT_EQ(client.is_running(), false);
    client.stop();
    EXPECT_EQ(client.is_running(), false);
}

TEST_F(ConnectionFixture, СonnectingСlienToServerAndReconnectionCheck){
    UserClient client(49152, "127.0.0.1");
    server.start();
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10 ));
    EXPECT_EQ(server.getConnections().size(), 1);
    server.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(12000));
}

TEST_F(ConnectionFixture, СonnectingСlienToServerAndDisconnectCheckClient){
    UserClient client(49152, "127.0.0.1");
    server.start();
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    client.stop();
    EXPECT_EQ(client.is_running(), false);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 0);
    EXPECT_EQ(server.is_running(), true);
    server.stop();
}

TEST_F(ConnectionFixture, СonnectingManyСlienToServerAndDisconnectCheckClient){
    UserClient client1(49152, "127.0.0.1");
    UserClient client2(49152, "127.0.0.1");
    UserClient client3(49152, "127.0.0.1");
    server.start();
    client1.start();
    client2.start();
    client3.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 3);
    client1.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 2);
    client2.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    client3.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 0);
    server.stop();
}

TEST_F(ConnectionFixture, SendingMessageToServer){
    UserClient client(49152, "127.0.0.1");
    server.start();
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    std::unique_ptr<BaseMessage> msg = std::make_unique<DisconnectMessage>("simple shutdown");
    client.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
    client.stop();
    server.stop();
}

TEST_F(ConnectionFixture, SendingManyMessageToServer){
    UserClient client(49152, "127.0.0.1");
    server.start();
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    std::unique_ptr<BaseMessage> msg = std::make_unique<DisconnectMessage>("simple shutdown");
    client.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
    msg = std::make_unique<SimpleMessage>("Hello");
    client.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    out = "Hello";
    log = getLastLog();
    EXPECT_EQ(log, out);
    msg = std::make_unique<SimpleMessage>("Bye");
    client.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    out = "Bye";
    log = getLastLog();
    EXPECT_EQ(log, out);
    client.stop();
    server.stop();
}

TEST_F(ConnectionFixture, SendingManyMessageToServerMultipleClients){
    UserClient client1(49152, "127.0.0.1");
    UserClient client2(49152, "127.0.0.1");
    UserClient client3(49152, "127.0.0.1");
    server.start();
    client1.start();
    client2.start();
    client3.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 3);
    std::unique_ptr<BaseMessage> msg = std::make_unique<DisconnectMessage>("simple shutdown");
    client1.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
    msg = std::make_unique<SimpleMessage>("Hello");
    client2.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    out = "Hello";
    log = getLastLog();
    EXPECT_EQ(log, out);
    msg = std::make_unique<SimpleMessage>("Bye");
    client3.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    out = "Bye";
    log = getLastLog();
    EXPECT_EQ(log, out);
    client1.stop();
    client2.stop();
    client3.stop();
    server.stop();
}

TEST_F(ConnectionFixture, SendingMessageToServerAndResponseToClient){
    UserClient client(49152, "127.0.0.1");
    server.start();
    client.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(server.getConnections().size(), 1);
    std::unique_ptr<BaseMessage> msg = std::make_unique<ForwardingMessage>("I'll be out soon");
    client.send_message(std::move(msg));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::string out = "Accepted";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
    client.stop();
    server.stop();
}