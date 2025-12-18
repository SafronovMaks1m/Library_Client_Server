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
    if (_running.load())
        return;
    _running.store(true);
    recv_msg_thread = std::thread(&Client::recv_message, this);
    send_msg_thread = std::thread(&Client::send_message_thd, this);
    connection_request();
}

void Client::stop(){
    if (!_running.load())
        return;
        
    _running.store(false);
    _recv_notify_queue.push(std::shared_ptr<Connection>{});
    _messages_cv_send.notify_one();
    _service.stop();
    if (recv_msg_thread.joinable()){
        recv_msg_thread.join();
    }
    if (send_msg_thread.joinable())
        send_msg_thread.join();
    std::lock_guard<std::mutex> lock(_connection_mutex);
    {
        if (_connection) 
            _connection->disconnect();
    }
    _service.reset();
    std::cout << "Client stopped." << std::endl;
}

const bool Client::is_running() const{
    return _running.load();
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
    while (_connection_attempts > 0 && _running.load()){
        try {
            tcp::endpoint ep(ip::address::from_string(_ip), _port);
            tcp::socket sock(_service);
            std::cout << "Connecting to " << _ip << ":" << _port << "..." << std::endl;
            
            sock.connect(ep);

            std::cout << "Connected successfully!" << std::endl;

            _connection = std::make_shared<Connection>(std::move(sock), _recv_notify_queue);

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
    if (!_running.load() || !_connection->is_running())
        return;

    _connection->_messages_send.push_wait(std::move(msg));

    _messages_cv_send.notify_one();
}

void Client::send_message_thd(){
    while (_running.load()) {
        std::unique_lock<std::mutex> ul(_messages_mutex_send);
        
        _messages_cv_send.wait(ul, [this]() { return (_connection && _connection->has_messages_send()) || !_running.load(); });

        if (!_running.load()) break;

        auto msg = _connection->pop_message_send();
        _messages_cv_send.notify_one();
        if (msg) {
            ul.unlock();
            _connection->send(std::move(msg));
        }
    }
}

void Client::recv_message() {
    while (_running.load()) {
        std::shared_ptr<Connection> conn;
        try {
            conn = _recv_notify_queue.wait_and_pop();
        } catch (...) {
            continue;
        }
        if (!conn) 
            continue;

        while (conn->is_running() && conn->has_messages_recv()) {
            auto msg = conn->pop_message_recv();
            if (!msg) 
                break;
            try {
                handler->handler(std::move(msg));
            } 
            catch (const std::exception& e) {
                std::cerr << "Client Error handling message: " << e.what() << std::endl;
            }
        }
    }
}

Client::~Client(){
    stop();
}