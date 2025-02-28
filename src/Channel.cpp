
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
int    Channel::addClient(Client* client, const string& key) {
    if (!this->_key.empty() && key != this->_key)
        return (-1);
    this->_clients[client->getSocket()] = client;
    client->addChannel(this->_name);
    return (0);
}
void    Channel::removeClient(Client client) {
    this->_clients.erase(client.getSocket());
}
void    Channel::addOperator(Client client) {
    this->_operators[client.getSocket()] = &client;
}
void    Channel::removeOperator(Client client) {
    this->_clients.erase(client.getSocket());
}

void    Channel::broadcastMessage(const string& message, Client client) {
    for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        if (it->first != client.getSocket()) {
            string formattedMessage = ":" + client.getNickname() + " PRIVMSG " + this->_name + " :" + message + "\r\n";
            send(it->first, formattedMessage.c_str(), formattedMessage.size(), MSG_NOSIGNAL);
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

Channel::Channel(const string& channelName, Client& client) :
_name(channelName), _inviteOnly(false), _topicLock(false), _userLimit(-1) {
    client.addChannel(channelName);
    addClient(&client, "");
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
