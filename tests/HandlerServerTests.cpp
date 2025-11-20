#include <gtest/gtest.h>
#include "UserServer.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

struct HandlerServerFixture : public testing::Test {
    UserSever server;
    HandlerServerFixture() : server(1111, "127.0.0.1") {}
    void SetUp() {
    }

    void TearDown() {
    }
};

TEST_F(HandlerServerFixture, HandlerServerMessagePing){
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    std::condition_variable parent_cv; std::mutex parent_mutex;
    Connection con(std::move(socket), parent_cv, parent_mutex);
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    server.handler->handler(std::make_unique<PingMessage>(milsec.count()), con);
    std::string out = "Send a pong via the send_message method on the passed connection, " 
        + std::to_string(milsec.count()) + " ms";
    EXPECT_EQ(getLastLog(), out);
}

TEST_F(HandlerServerFixture, HandlerServerMessagePong){
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    std::condition_variable parent_cv; std::mutex parent_mutex;
    Connection con(std::move(socket), parent_cv, parent_mutex);
    auto now = std::chrono::system_clock::now();
    auto milsec = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    server.handler->handler(std::make_unique<PongMessage>(milsec.count()), con);
    std::string log = getLastLog();
    EXPECT_NE(log.find("Ping ="), std::string::npos);
    EXPECT_NE(log.find("ms"), std::string::npos);
}

TEST_F(HandlerServerFixture, HandlerServerMessageDisconnect){
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    std::condition_variable parent_cv; std::mutex parent_mutex;
    Connection con(std::move(socket), parent_cv, parent_mutex);
    server.handler->handler(std::make_unique<DisconnectMessage>("simple shutdown"), con);
    std::string out = "Client disconnected, reason: simple shutdown";
    std::string log = getLastLog();
    EXPECT_EQ(log, out);
}
