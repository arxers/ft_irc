#include "../inc/Client.hpp"

// private:


// public:
void    Client::authenticate() {
    if (this->_state < AUTHENTICATED)
        this->_state = AUTHENTICATED;
}

void    Client::addChannel(const string& channel) {
    this->_channels.push_back(channel);
}
void    Client::removeChannel(const string& channel) {
    (void)channel;
}

void    Client::sendMessage(const string& message, const string& sender) {
    string formattedMessage = ":" + sender + " PRIVMSG " + this->_nickname + " :" + message + "\r\n";
        send(this->_socketFd, formattedMessage.c_str(), formattedMessage.size(), MSG_NOSIGNAL);
}

// Predicates, getters, setters

bool    Client::isOperator() const {
    return (this->_op == true);
}

bool    Client::isAuthenticated() const {
    return (this->_state >= AUTHENTICATED);
}

bool    Client::isConnected() const {
    return (this->_socketFd != -1);
}

bool    Client::isInChannel(const string& channel) {
    vector<string>::iterator it = std::find(this->_channels.begin(), this->_channels.begin(), channel);

    if (it != this->_channels.end())
        return (true);
    return (false);
}

int Client::getSocket() const {
    return (this->_socketFd);
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

e_client_state  Client::getState() const {
    return (this->_state);
}

const string&  Client::getNickname() const {
    return (this->_nickname);
}
const string&  Client::getUsername() const {
    return (this->_username);
}
const vector<string>& Client::getChannels() const {
    return (this->_channels);
}

void    Client::setState(e_client_state state) {
    this->_state = state;
}

void    Client::setNickname(const string& nickname) {
    this->_nickname = nickname;
}

void    Client::setUsername(const string& username) {
    this->_username = username;
}

void    Client::setRealname(const string& realname) {
    this->_realname = realname;
}

Client::Client() {}

Client::Client(int socketFd, struct sockaddr_in addr) :
_socketFd(socketFd),
_addr(addr),
_state(CONNECTED),
_op(false),
_nickname("*")
{}

Client::Client(const Client& rhs) {
    if (this != &rhs)
        *this = rhs;
}

Client::~Client() {}

Client& Client::operator=(const Client& rhs) {
    if (this != &rhs) {
        this->_socketFd = rhs._socketFd;
        this->_addr = rhs._addr;
        this->_inputBuffer = rhs._inputBuffer;
        this->_outputBuffer = rhs._outputBuffer;
        this->_state = rhs._state;
        this->_op = rhs._op;
        this->_nickname = rhs._nickname;
        this->_username = rhs._username;
        this->_realname = rhs._realname;
        this->_password = rhs._password;
        this->_channels = rhs._channels;
    }
    return (*this);
}
