
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

bool    Channel::isEmpty() const {
    return (!this->_userCount);
}

bool    Channel::isInviteOnly() const {
    return (this->_inviteOnly);
}

bool    Channel::isTopicLocked() const {
    return (this->_topicLock);
}

bool    Channel::isClientInvited(Client& client) const {
    return (this->_invitees.find(client.getSocket()) != this->_invitees.end());
}

bool    Channel::hasClient(const string& nickname) const {
    for (map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it)
        if (it->second->getNickname() == nickname)
            return (true);
    return (false);
}

bool    Channel::isClientOp(const Client& client) const {
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
    if (this->_userLimit && this->_userCount >= this->_userLimit)
        return (ERR_CHANNELISFULL);
    this->_clients[client.getSocket()] = &client;
    client.addChannel(*this);
    this->_userCount++;
    std::cout << client.getPrefix() << " joined " << this->_name << ". User count: " << _userCount << '\n';
    return (RPL_SUCCESS);
}
void    Channel::removeClient(Client& client) {
    client.removeChannel(this->_name);
    this->_clients.erase(client.getSocket());
    this->_userCount--;
    std::cout << client.getPrefix() << " left " << this->_name << ". User count: " << _userCount << '\n';
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
    if (this->_userCount < 1)
        return ;
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        string  formattedMessage = ":" + sender + " " + command + " " + this->_name + " :" + message + "\r\n";
        string& clientBuffer = it->second->getOutputBuffer();
        clientBuffer += formattedMessage;
    }
}

// :jaslim!~j@203.149.201.178 PART #testtestaaa
// :jjjj!~j@203.149.201.178 PART #test123123 :param

void    Channel::broadcastMessage(const string& message, const string& command, const Client& sender) {
    if (this->_userCount < 1)
        return ;
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        if (it->first != sender.getSocket()) {
            string formattedMessage = ":" + sender.getNickname() + " " + command + " " + this->_name + " :" + message + "\r\n";
            string& clientBuffer = it->second->getOutputBuffer();
            clientBuffer += formattedMessage;
        }
    }
}

void    Channel::broadcastMessage(const string& message) {
    if (this->_userCount < 1)
        return ;
    for (std::map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        Client* client = it->second;
        if (!client)
            continue ;
        string& clientBuffer = client->getOutputBuffer();
        clientBuffer += message + CRLF;
    }
}

void    Channel::setInviteOnly(bool inviteOnly){
    this->_inviteOnly = inviteOnly;
}

void    Channel::setTopicLock(bool topicLock) {
    this->_topicLock = topicLock;
}

void    Channel::setTopic(const string& topic) {
    this->_topic = topic;
}

void    Channel::setUserLimit(int userLimit) {
    this->_userLimit = userLimit;
}

void    Channel::setKey(const string& key) {
    this->_key = key;
}

string Channel::getNamesList() const {
    string namesList;

    for (map<int, Client*>::const_iterator it = this->_clients.begin(); it != this->_clients.end(); ++it) {
        Client& client = *it->second;
        string name = client.getNickname();
        if (this->isClientOp(client))
            name = "@" + name;
        namesList += name + " ";
    }

    if (!namesList.empty())
        namesList.resize(namesList.size() - 1);

    return namesList;
}


const string& Channel::getName() const{
    return (this->_name);
}

const string& Channel::getKey() const{
    return (this->_key);
}

const string& Channel::getTopic() const {
    return (this->_topic);
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

Channel::Channel(const string& channelName, Client& client, set<int> invitees) :
_name(channelName), _invitees(invitees), _inviteOnly(false), _topicLock(false), _userLimit(0), _userCount(0) {
    client.addChannel(*this);
    addClient(client, "");
    addOperator(client);
}

Channel::Channel(const string& channelName, Client& client) :
_name(channelName), _inviteOnly(false), _topicLock(false), _userLimit(0), _userCount(0) {
    client.addChannel(*this);
    addClient(client, "");
    addOperator(client);
}

Channel::Channel(const Channel& rhs) {
    if (this != &rhs)
        *this = rhs;
}

Channel& Channel::operator=(const Channel& rhs) {
    if (this != &rhs) {
        this->_name = rhs._name;
        this->_clients = rhs._clients;
        this->_operators = rhs._operators;
        this->_invitees = rhs._invitees;
        this->_inviteOnly = rhs._inviteOnly;
        this->_topicLock = rhs._topicLock;
        this->_userLimit = rhs._userLimit;
        this->_userCount = rhs._userCount;
        this->_key = rhs._key;
        this->_topic = rhs._topic;
    }
    return (*this);
}

Channel::~Channel() {}
