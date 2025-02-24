
#include "../inc/Channel.hpp"

// private:
// clientmap_t _clients;
// const string    _name;
// clientmap_t _operators;         // o
// bool        _inviteOnly;        // i
// bool        _topicRestricted;   // t
// int         _userLimit;         // l
// string      _key;               // k

Channel::Channel() {}
Channel::Channel(const Channel&) {}
Channel& Channel::operator=(const Channel&) { return (*this); }

// public:
void    Channel::addClient(Client client) {
    this->_clients[client.getSocket()] = client;
}
void    Channel::removeClient(Client client) {
    this->_clients.erase(client.getSocket());
}
void    Channel::addOperator(Client client) {
    this->_operators[client.getSocket()] = client;
}
void    Channel::removeOperator(Client client) {
    this->_clients.erase(client.getSocket());
}

void    Channel::setKey(const string& key) {
    this->_key = key;
}

const string& Channel::getKey() {
    return (this->_key);
}

Channel::Channel(const string& channelName, Client client) :
_name(channelName), _inviteOnly(false), _topicLock(false), _userLimit(-1) {
    addClient(client);
    addOperator(client);
}
Channel::~Channel() {}
