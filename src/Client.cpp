#include "../inc/Client.hpp"

// private:


// public:

void    Client::addChannel(Channel& channel) {
    string channelName = channel.getName();
    this->_channels.insert(channelName);
}

void    Client::removeChannel(const string& channel) {
    this->_channels.erase(channel);
}

void    Client::sendMessage(const string& message, const string& sender) {
    string formattedMessage = ":" + sender + " PRIVMSG " + this->_nickname + " :" + message + "\r\n";
    this->_outputBuffer += formattedMessage;
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

bool    Client::isPinged() const {
    return (this->_pinged);
}

bool    Client::isInChannel(const string& channel) const {
    set<string>::const_iterator it = this->_channels.find(channel);
    if (it != this->_channels.end())
        return (true);
    return (false);
}

string&    Client::getInputBuffer() {
    return (this->_inputBuffer);
}

string&    Client::getOutputBuffer() {
    return (this->_outputBuffer);
}

int Client::getSocket() const {
    return (this->_socketFd);
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

string  Client::getPrefix() const {
    return (this->_nickname + "!~" + this->_username + "@" + inet_ntoa(this->_addr.sin_addr));
}

set<string> Client::getChannels() const {
    return (this->_channels);
}

time_t  Client::getIdleTime() const{
    return (time(NULL) - this->_lastActiveTime);
}

time_t  Client::getTimeSinceLastPing() const{
    return (time(NULL) - this->_lastPingTime);
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

void    Client::setLastActiveTime() {
    this->_lastActiveTime = time(NULL);
}

void    Client::setLastPingTime() {
    this->_lastPingTime = time(NULL);
}

void    Client::setPinged(bool pinged) {
    this->_pinged = pinged;
}

Client::Client() {}

Client::Client(int socketFd, struct sockaddr_in addr) :
_socketFd(socketFd),
_addr(addr),
_state(CONNECTED),
_op(false),
_nickname("*"),
_ip(inet_ntoa(addr.sin_addr)),
_lastActiveTime(time(NULL)),
_lastPingTime(_lastActiveTime),
_pinged(false)
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
        this->_ip = rhs._ip;
        this->_lastActiveTime = rhs._lastActiveTime;
        this->_lastPingTime = rhs._lastPingTime;
        this->_pinged = rhs._pinged;
        this->_channels = rhs._channels;
    }
    return (*this);
}
