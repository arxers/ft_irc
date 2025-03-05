
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

bool    Channel::isInviteOnly() const {
    return (this->_inviteOnly);
}

bool    Channel::isTopicLocked() const {
    return (this->_topicLock);
}

bool    Channel::isClientInvited(Client& client) const {
    return (this->_invitees.find(client.getSocket()) != this->_invitees.end());
}

bool    Channel::isClientInChannel(const string& nickname) const {
    for (map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
        if (it->second->getNickname() == nickname)
            return (true);
    return (false);
}

bool    Channel::isClientOp(Client& client) const {
    return (this->_operators.find(client.getSocket()) != this->_operators.end());
}

bool    Channel::hasChannelKey() const {
    return (!this->_key.empty());
}

bool    Channel::hasUserLimit() const {
    return (this->_userLimit > 0);
}

int    Channel::addClient(Client& client, const string& key) {
    if (this->_inviteOnly && !this->isClientInvited(client))
        return (ERR_INVITEONLYCHAN);
    if (!this->_key.empty() && key != this->_key)
        return (ERR_BADCHANNELKEY);
    if (this->_userCount > this->_userLimit)
        return (ERR_CHANNELISFULL);
    this->_clients[client.getSocket()] = &client;
    client.addChannel(*this);
    return (RPL_SUCCESS);
}
void    Channel::removeClient(Client& client) {
    client.removeChannel(this->_name);
    this->_clients.erase(client.getSocket());
}

void    Channel::addInvitee(Client& client) {
    this->_invitees.insert(client.getSocket());
}

void    Channel::removeInvitee(Client& client) {
    this->_invitees.erase(client.getSocket());
}

void    Channel::addOperator(Client& client) {
    this->_operators.insert(client.getSocket());
}
void    Channel::removeOperator(Client& client) {
    this->_operators.erase(client.getSocket());
}

void    Channel::broadcastMessage(const string& message, const string& command, const string& sender) {
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        string  formattedMessage = ":" + sender + " " + command + " " + this->_name + " :" + message + "\r\n";
        string& clientBuffer = it->second->getOutputBuffer();
        clientBuffer += formattedMessage;
    }
}

// :jaslim!~j@203.149.201.178 PART #testtestaaa
// :jjjj!~j@203.149.201.178 PART #test123123 :param

void    Channel::broadcastMessage(const string& message, const string& command, const Client& sender) {
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        if (it->first != sender.getSocket()) {
            string formattedMessage = ":" + sender.getNickname() + " " + command + " " + this->_name + " :" + message + "\r\n";
            string& clientBuffer = it->second->getOutputBuffer();
            clientBuffer += formattedMessage;
        }
    }
}

void    Channel::setInviteOnly(bool inviteOnly){
    this->_inviteOnly = inviteOnly;
}

void    Channel::setTopicLock(bool topicLock) {
    this->_topicLock = topicLock;
}

void    Channel::setUserLimit(int userLimit) {
    this->_userLimit = userLimit;
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

int Channel::getUserLimit() const {
    return (this->_userLimit);
}

Client* Channel::getClient(const string& nickname) {
    for (map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
        if (it->second->getNickname() == nickname)
            return (it->second);
    return (NULL);
}

Channel::Channel() {}

Channel::Channel(const string& channelName, Client& client) :
_name(channelName), _inviteOnly(false), _topicLock(false), _userLimit(0) {
    client.addChannel(*this);
    addClient(client, "");
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
