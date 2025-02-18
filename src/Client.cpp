#include "../inc/Client.hpp"

// private:


// public:
void    Client::authenticate() {
    this->_authenticated = true;
}

void    Client::addChannel(const string& channel) {
    (void)channel;
}
void    Client::removeChannel(const string& channel) {
    (void)channel;
}

// Predicates, getters, setters

bool    Client::isOperator() const {
    return (this->_op == true);
}

bool    Client::isAuthenticated() const {
    return (this->_authenticated == true);
}

bool    Client::isConnected() const {
    return (this->_socket_fd != -1);
}

int Client::getSocket() const {
    return (this->_socket_fd);
}

string&    Client::getInputBuffer() {
    return (this->_inputBuffer);
}

string&    Client::getOutputBuffer() {
    return (this->_outputBuffer);
}

const string  Client::getIp() const {
    return (inet_ntoa(this->_addr.sin_addr));
}

const string&  Client::getNickname() const {
    return (this->_nickname);
}
const string&  Client::getUsername() const {
    return (this->_username);
}
const std::vector<string>& Client::getChannels() const {
    return (this->_channels);
}

void    Client::setNickname(const string& nickname) {
    this->_nickname = nickname;
}

Client::Client() : _socket_fd(0), _authenticated(false)
{}

Client::Client(int socket_fd, struct sockaddr_in addr) :
_socket_fd(socket_fd),
_addr(addr),
_op(false),
_authenticated(false)
{}

Client::Client(const Client& rhs) {
    if (this != &rhs)
        *this = rhs;
}

Client::~Client() {}

Client& Client::operator=(const Client& rhs) {
    if (this != &rhs) {
        this->_socket_fd = rhs._socket_fd;
        this->_addr = rhs._addr;
        this->_op = rhs._op;
        this->_authenticated = rhs._authenticated;
        this->_nickname = rhs._nickname;
        this->_username = rhs._username;
        this->_password = rhs._password;
        this->_channels = rhs._channels;
    }
    return (*this);
}
