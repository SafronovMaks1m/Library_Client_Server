#include <mutex>
#include "Client.h"
#include "HandlerMessageClient.h"

Client::Client(uint16_t port, std::string ip): _port(port), _ip(ip), _running(false), _connection_attempts(5), _service(){
    handler = std::make_unique<HandlerMessageClient>(*this);
}

const std::shared_ptr<Connection>& Client::getConnection() const{
    std::lock_guard<std::mutex> lock(_connection_mutex);
    return _connection;
}

void Client::start(){
    if (_running)
        return;
    _running = true;
    recv_msg_thread = std::thread(&Client::recv_message, this);
    connection_request();
}

void Client::stop(){
    std::lock_guard<std::mutex> lock(_stop_mutex);
    if (!_running)
        return;
        
    _running = false;
    _messages_cv.notify_one();
    if (recv_msg_thread.joinable()){
        recv_msg_thread.join();
    }
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);
        if(_connection){
            _connection->disconnect();
            _connection.reset();
        }
    }
    _service.stop();

    std::cout << "Client stopped." << std::endl;
}

const bool Client::is_running() const{
    return _running;
}

void Client::reconnecting(){
    bool expected = false;
    if (!_reconnecting.compare_exchange_strong(expected, true)) {
        return;
    }
    std::cout << "Attempting to reconnect..." << std::endl;
    
    {
        std::lock_guard<std::mutex> lock(_connection_mutex);
        if (_connection) {
            _connection->disconnect();
            _connection.reset();
        }
    }
    connection_request();

    _reconnecting = false;
}

void Client::connection_request(){
    while (_connection_attempts > 0 && _running){
        try {
            tcp::endpoint ep(ip::address::from_string(_ip), _port);
            tcp::socket sock(_service);
            std::cout << "Connecting to " << _ip << ":" << _port << "..." << std::endl;
            
            sock.connect(ep);

            std::cout << "Connected successfully!" << std::endl;

            _connection = std::make_shared<Connection>(std::move(sock), _messages_cv, _messages_mutex);

            _connection->on_disconnect = [this]() { reconnecting(); };
            _connection->on_error = [this]() { stop(); };

            _connection->start_recv();
            
            _connection_attempts = 5;
            return;
        } 
        catch (const boost::system::system_error& e) {
            std::cerr << "Connection failed: " << e.what() << std::endl;
            _connection_attempts -= 1;
            std::cout << "Reconnecting in 2 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));

        } 
        catch (const std::exception& e) {
            std::cerr << "Unexpected error: " << e.what() << std::endl;
            _connection_attempts = 0;
            break;
        }
    }
    if (!_connection_attempts){
        std::cerr << "Failed to connect..." << std::endl;
        stop();
    }
}

void Client::send_message(std::unique_ptr<BaseMessage>&& msg) {
    std::lock_guard<std::mutex> lock(_connection_mutex);
    if (_connection) {
        _connection->send(std::move(msg));
    }
}

void Client::recv_message() {
    while (_running) {
        std::unique_lock<std::mutex> ul(_messages_mutex);
        
        _messages_cv.wait(ul, [this]() { return (_connection && _connection->has_messages()) || !_running; });


        if (!_running) break;

        auto msg = _connection->pop_message();

        if (msg) {
            ul.unlock();

            try {
                handler->handler(std::move(msg));
            } 
            catch (const std::exception& e) {
                std::cerr << "Client Error handling message: " << e.what() << std::endl;
            }

            ul.lock();
        }
    }
}

Client::~Client(){
    stop();
}