#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <mutex>
#include <iostream>
#include "Message.h"
#include "Serealize.h"


using boost::asio::ip::tcp;

class Client;

class Connection : public std::enable_shared_from_this<Connection>{
    private:
        tcp::socket _socket;
        std::thread recv_thread;
        bool _running;
    public:
        explicit Connection(tcp::socket&& socket);

        template <class T>
        void send(std::unique_ptr<BaseMessage>&& msg, T& obj){
            try {
                std::string data = Serealize::serealizer(*msg);

                uint32_t size = static_cast<uint32_t>(data.size());
                size = htonl(size);
                uint16_t type = msg->getType();
                type = htons(type);

                boost::asio::write(_socket, boost::asio::buffer(&size, sizeof(size)));
                boost::asio::write(_socket, boost::asio::buffer(&type, sizeof(type)));
                boost::asio::write(_socket, boost::asio::buffer(data));
            }
            catch (const boost::system::system_error& e) {
                if (_running) {
                    std::cerr << "[Recv] Error on connection " << _socket.remote_endpoint() 
                            << ": " << e.what() << " (code=" << e.code() << ")" << std::endl;
                    if constexpr (std::is_same_v<T, Client>) {
                        std::thread([&obj]() { obj.reconnecting(); }).detach();
                    } 
                    else{
                        std::thread([this, &obj]() { obj.disconnect(*this); }).detach();
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[Send] Unexpected error: " << e.what() << std::endl;
                std::thread([&obj]() { obj.stop(); }).detach();
            }
            catch (...) {
                std::cerr << "[Send] Unknown exception occurred." << std::endl;
                std::thread([&obj]() { obj.stop(); }).detach();
            }
        }

        template <class T>
        void start_recv(T& obj){
            _running = true;
            recv_thread = std::thread([this, &obj]() {this->recv(obj);});
        }

        template <class T>
        void recv(T& obj){
            try {
                while (_running && _socket.is_open()) {
                    uint32_t size;
                    boost::asio::read(_socket, boost::asio::buffer(&size, sizeof(size)));
                    size = ntohl(size);

                    uint16_t type;
                    boost::asio::read(_socket, boost::asio::buffer(&type, sizeof(type)));
                    type = ntohs(type);

                    std::vector<char> buffer(size);
                    boost::asio::read(_socket, boost::asio::buffer(buffer.data(), buffer.size()));
                    std::string data(buffer.begin(), buffer.end());

                    if (Serealize::deserealizers.find(type) != Serealize::deserealizers.end()) {
                        auto msg = Serealize::deserealizers[type](data);
                        obj.add_message(shared_from_this(), std::move(msg));
                    } 
                    else {
                        std::cerr << "Unknown message type: " << type << std::endl;
                    }
                }
            } 
            catch (const boost::system::system_error& e) {
                if (_running) {
                    std::cerr << "[Recv] Error on connection " << _socket.remote_endpoint() 
                            << ": " << e.what() << " (code=" << e.code() << ")" << std::endl;
                    if constexpr (std::is_same_v<T, Client>) {
                        std::thread([&obj]() { obj.reconnecting(); }).detach();
                    } 
                    else{
                        std::thread([this, &obj]() { obj.disconnect(*this); }).detach();
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[Recv] Unexpected error: " << e.what() << std::endl;
                std::thread([&obj]() { obj.stop(); }).detach();
            }
            catch (...) {
                std::cerr << "[Recv] Unknown exception occurred." << std::endl;
                std::thread([&obj]() { obj.stop(); }).detach();
            }
        }

        void disconnect();
};