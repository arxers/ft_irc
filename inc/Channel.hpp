#pragma once

#include "Client.hpp"

class Channel {
private:
    const string    _name;
    clientmap_t     _clients;
    clientmap_t     _operators;     // o
    bool            _inviteOnly;    // i
    bool            _topicLock;     // t
    int             _userLimit;     // l
    string          _key;           // k

    Channel();
    Channel(const Channel& rhs);
    Channel& operator=(const Channel& rhs);
public:
    void    addClient(Client client);
    void    removeClient(Client client);
    void    addOperator(Client client);
    void    removeOperator(Client client);

    void    setKey(const string& key);

    const string& getKey();

    Channel(const string& channelName, Client client);
    ~Channel();
};

typedef std::map<string, Channel>    channelmap_t;
