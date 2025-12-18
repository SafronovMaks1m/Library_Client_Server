#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <functional>
#include <iostream>
#include <queue>
#include <atomic>
#include "Message.h"
#include "ThreadSafeQueue.h"
#include "Serealize.h"


using boost::asio::ip::tcp;

class Client;

class Connection : public std::enable_shared_from_this<Connection>{
    private:
        tcp::socket _socket;
        std::thread recv_thread;
        std::atomic<bool> _running;

        ThreadSafeQueue<std::unique_ptr<BaseMessage>> _messages_recv;
        ThreadSafeQueue<std::shared_ptr<Connection>>& _own_notify;

    public:
        ThreadSafeQueue<std::unique_ptr<BaseMessage>> _messages_send;
        std::function<void()> on_disconnect;
        std::function<void()> on_error;
    
        explicit Connection(tcp::socket&& socket, ThreadSafeQueue<std::shared_ptr<Connection>>& own_notify);

        void send(std::unique_ptr<BaseMessage>&& msg){
            try {
                uint16_t type = msg->getType(); 
                std::string data = Serealize::serealizer(*msg);
                uint32_t size = static_cast<uint32_t>(data.size());
                size = htonl(size);
                type = htons(type);

                boost::asio::write(_socket, boost::asio::buffer(&size, sizeof(size)));
                boost::asio::write(_socket, boost::asio::buffer(&type, sizeof(type)));
                boost::asio::write(_socket, boost::asio::buffer(data));
            }
            catch (const boost::system::system_error& e) {
                if (_running.load()) {
                    std::cerr << "[Recv] Error on connection " << _socket.remote_endpoint() 
                            << ": " << e.what() << " (code=" << e.code() << ")" << std::endl;
                    std::thread([this]() { on_disconnect(); }).detach();
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[Send] Unexpected error: " << e.what() << std::endl;
                std::thread([this]() { on_error(); }).detach();
            }
            catch (...) {
                std::cerr << "[Send] Unknown exception occurred." << std::endl;
                std::thread([this]() { on_error(); }).detach();
            }
        }

        void start_recv(){
            _running.store(true);
            recv_thread = std::thread([this]() {this->recv();});
        }

        void recv(){
            try {
                while (_running.load() && _socket.is_open()) {
                    uint32_t size;
                    boost::asio::read(_socket, boost::asio::buffer(&size, sizeof(size)));
                    size = ntohl(size);

                    uint16_t type;
                    boost::asio::read(_socket, boost::asio::buffer(&type, sizeof(type)));
                    type = ntohs(type);

                    std::vector<char> buffer(size);
                    boost::asio::read(_socket, boost::asio::buffer(buffer.data(), buffer.size()));
                    std::string data(buffer.begin(), buffer.end());
                    auto msg = Serealize::deserealizer(data);
                    _messages_recv.push_wait(std::move(msg));
                    _own_notify.push(shared_from_this());
                }
            } 
            catch (const boost::system::system_error& e) {
                if (_running.load()) {
                    std::cerr << "[Recv] Error on connection " << _socket.remote_endpoint() 
                            << ": " << e.what() << " (code=" << e.code() << ")" << std::endl;
                    std::thread([this]() { on_disconnect(); }).detach();
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[Recv] Unexpected error: " << e.what() << std::endl;
                std::thread([this]() { on_error(); }).detach();
            }
            catch (...) {
                std::cerr << "[Recv] Unknown exception occurred." << std::endl;
                std::thread([this]() { on_error(); }).detach();
            }
        }
        
        inline bool has_messages_recv() const {
            return !_messages_recv.empty();
        }

        inline bool has_messages_send() const {
            return !_messages_send.empty();
        }

        bool is_running();

        std::unique_ptr<BaseMessage> pop_message_recv();

        std::unique_ptr<BaseMessage> pop_message_send();

        void disconnect();
};