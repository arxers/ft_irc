#pragma once

#include "Client.hpp"

using std::string;
using std::map;

class Client;

class Channel {
private:
    string              _name;
    map<int, Client*>    _clients;
    map<int, Client*>    _operators;     // o
    bool                _inviteOnly;    // i
    bool                _topicLock;     // t
    int                 _userLimit;     // l
    string              _key;           // k

public:
    int     addClient(Client* client, const string& key);
    void    removeClient(Client client);
    void    addOperator(Client client);
    void    removeOperator(Client client);

    void    broadcastMessage(const string& message, const string& command, const string& sender);
    void    broadcastMessage(const string& message, const string& command, const Client& client);

    void    setKey(const string& key);

    const string&   getName() const;
    const string&   getKey() const;

    Channel();
    Channel(const string& channelName, Client& client);
    Channel(const Channel& rhs);
    Channel& operator=(const Channel& rhs);
    ~Channel();
};

typedef std::map<string, Channel>    channelmap_t;
