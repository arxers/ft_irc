
#include "../inc/Channel.hpp"

    // string        _name;
    // map<int, Client>    _clients;
    // map<int, Client>    _operators;     // o
    // bool                _inviteOnly;    // i
    // bool                _topicLock;     // t
    // int                 _userLimit;     // l
    // string              _key;           // k
#include <iostream> //test


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

void    Channel::broadcastMessage(const string& message, Client client) {
    for (clientmap_t::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        std::cout << it->second.getNickname() << '\n';
        if (it->first != client.getSocket()) {
            std::cout << "sending to " << it->first << ": \"" << message << "\"\n";
            send(it->first, message.c_str(), message.size(), 0);
            // YOU NEED TO SEND BACK IN A PROPER FORMAT!!!
        }
    }
}

void    Channel::setKey(const string& key) {
    this->_key = key;
}

const string& Channel::getName() const{
    return (this->_name);
}

const string& Channel::getKey() const{
    return (this->_key);
}

Channel::Channel() {}

Channel::Channel(const string& channelName, Client client) :
_name(channelName), _inviteOnly(false), _topicLock(false), _userLimit(-1) {
    addClient(client);
    addOperator(client);
}

Channel::Channel(const Channel& rhs) {
    *this = rhs;
}

Channel& Channel::operator=(const Channel& rhs) {
    if (this != &rhs) {
        this->_name = rhs._name;
        this->_clients = rhs._clients;
        this->_operators = rhs._operators;
        this->_inviteOnly = rhs._inviteOnly;
        this->_topicLock = rhs._topicLock;
        this->_userLimit = rhs._userLimit;
        this->_key = rhs._key;
    }
    return (*this);
}

Channel::~Channel() {}
